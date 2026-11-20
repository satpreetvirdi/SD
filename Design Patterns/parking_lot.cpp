enum VehicleSize {SMALL , MEDIUM ,LARGE};

class Vehicle{
    protected: 
    string plate;
    public:
    Vehicle(string p){plate = p;}
    virtual ~Vehicle(){}
    string getplate(){return plate;}
    virtual VehicleSize getsize() = 0 ;
}

class Motorcycle : public Vehicle {
    public : 
    Motorcycle(string p) : Vehicle(p){}
    VehicleSize getsize() override {return "SMALL";}


}
class Car : public Vehicle {
    public :
    Car(string p) : Vehile(p){}
    VehicleSize getsize() override {return "MEDIUM";}
}
class Truck : public Vehicle {
    public :
    Truck(string p) : Vehicle(p){}
    VehicleSize getsize() override {return "LARGE";}
}

class ParkingSpot{
    protected:
    int number;
    Vehicle* vehicle = nullptr;
    public:
    ParkingSpot(int n){number = n;}
    virtual ~ParkingSpot(){}
    bool isFree()(return vehicle == nullptr;)
    void park(Vehicle* v){vehicle = v;}
    void leave()(vehicle = nullptr;)
    int getNumber(){ return number;}
    virtual VehicleSize getSize() =0;
}

class CompactSpot : public ParkingSpot{
    public:
    CompactSpot(int n) : ParkingSpot(n){}
    VehicleSize getsize() override {return SMALL}
}
class RegularSize : public ParkingSpot{
    public:
    RegularSize(int n) : ParkingSpot(n){}
    VehicleSize getsize() override {return REGULAR}
}
class OversizedSpot : public ParkingSpot{
    public:
    OversizedSpot(int n) : ParkingSpot(n){}
    VehicleSize getsize() override {return LARGE}
}
class ParkingManager {
    vector<ParkingSpot*> spots;
    map<string, ParkingSpot*> vehicleToSpot;    // plate -> spot
public:
    void addSpot(ParkingSpot* s) { spots.push_back(s); }

    // Best fit: try the vehicle's own size first, then bigger sizes
    ParkingSpot* findSpot(Vehicle* v) {
        for (int size = v->getSize(); size <= LARGE; size++) {
            for (ParkingSpot* s : spots) {
                if (s->isFree() && s->getSize() == size) return s;
            }
        }
        return nullptr;                         // no spot available
    }

    ParkingSpot* parkVehicle(Vehicle* v) {
        ParkingSpot* s = findSpot(v);
        if (s == nullptr) return nullptr;
        s->park(v);
        vehicleToSpot[v->getPlate()] = s;
        return s;
    }

    void unparkVehicle(Vehicle* v) {
        ParkingSpot* s = vehicleToSpot[v->getPlate()];
        s->leave();
        vehicleToSpot.erase(v->getPlate());
    }
};

// ---------- 4. TICKET (the receipt) ----------
class Ticket {
public:
    int id;
    Vehicle* vehicle;
    ParkingSpot* spot;
    int entryTime;                              // minutes since midnight
    int exitTime = 0;

    Ticket(int i, Vehicle* v, ParkingSpot* s, int entry) {
        id = i; vehicle = v; spot = s; entryTime = entry;
    }
    int duration() { return exitTime - entryTime; }   // in minutes
};
// ---------- 5. FARE (STRATEGY PATTERN) ----------
class FareStrategy {
public:
    virtual ~FareStrategy() {}
    // takes the fare so far, returns the new fare
    virtual double calculate(Ticket* t, double fare) = 0;
};

class BaseFare : public FareStrategy {          // rate per minute by size
public:
    double calculate(Ticket* t, double fare) override {
        double rate;
        if (t->vehicle->getSize() == SMALL)       rate = 1.0;
        else if (t->vehicle->getSize() == MEDIUM) rate = 2.0;
        else                                      rate = 3.0;
        return fare + rate * t->duration();
    }
};

class PeakHoursFare : public FareStrategy {     // x1.5 if entered 7-10 AM or 4-7 PM
public:
    double calculate(Ticket* t, double fare) override {
        int hour = t->entryTime / 60;
        bool peak = (hour >= 7 && hour < 10) || (hour >= 16 && hour < 19);
        return peak ? fare * 1.5 : fare;
    }
};

class FareCalculator {
    vector<FareStrategy*> strategies;           // applied in order
public:
    void addStrategy(FareStrategy* s) { strategies.push_back(s); }
    double calculate(Ticket* t) {
        double fare = 0;
        for (FareStrategy* s : strategies) fare = s->calculate(t, fare);
        return fare;
    }
};


// ---------- 6. PARKING LOT (FACADE: the gate) ----------
class ParkingLot {
    ParkingManager* manager;
    FareCalculator* calculator;
    int nextTicketId = 1;
public:
    ParkingLot(ParkingManager* m, FareCalculator* c) { manager = m; calculator = c; }

    Ticket* enter(Vehicle* v, int time) {
        ParkingSpot* s = manager->parkVehicle(v);
        if (s == nullptr) {
            cout << v->getPlate() << ": no spot available\n";
            return nullptr;
        }
        cout << v->getPlate() << " parked at spot " << s->getNumber() << "\n";
        return new Ticket(nextTicketId++, v, s, time);
    }

    double exit(Ticket* t, int time) {
        t->exitTime = time;
        double fare = calculator->calculate(t);
        manager->unparkVehicle(t->vehicle);
        cout << t->vehicle->getPlate() << " left, fare = " << fare << "\n";
        delete t;
        return fare;
    }
};


int main() {
    // build the lot: one spot of each size
    CompactSpot s1(1);
    RegularSpot s2(2);
    OversizedSpot s3(3);
    ParkingManager manager;
    manager.addSpot(&s1);
    manager.addSpot(&s2);
    manager.addSpot(&s3);

    // pricing rules: base first, then peak
    BaseFare base;
    PeakHoursFare peak;
    FareCalculator calculator;
    calculator.addStrategy(&base);
    calculator.addStrategy(&peak);

    ParkingLot lot(&manager, &calculator);

    Motorcycle bike("BIKE-1");
    Car car("CAR-1");
    Truck truck("TRUCK-1");
    Car car2("CAR-2");

    Ticket* t1 = lot.enter(&bike,  12 * 60);    // 12:00
    Ticket* t2 = lot.enter(&car,    8 * 60);    // 08:00 (peak)
    Ticket* t3 = lot.enter(&truck, 13 * 60);    // 13:00
    lot.enter(&car2, 13 * 60);                  // lot full for a car

    lot.exit(t1, 12 * 60 + 30);                 // 30 min x 1.0 = 30
    lot.exit(t2,  9 * 60);                      // 60 min x 2.0 x 1.5 = 180
    lot.exit(t3, 14 * 60);                      // 60 min x 3.0 = 180
    return 0;
}