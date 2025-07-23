#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>

using namespace std;

struct Cars {
    string mark[10] = { "Honda", "BMW", "Mercedes", "Audi", "Tesla", "Proton", "Volkswagen", "Perodua", "Toyota" };
    string model[10] = { "2016MS", "2018AS", "2018DS", "2017LA", "2018OW", "2005Ps", "2008PW", "2010MS", "2016JN" };
    string color[10] = { "Red", "Black", "Yellow", "Blue", "Red", "Brown", "Silver", "Black", "Grey" };
    string maxSpeed[10] = { "100 Km/h", "150 Km/h", "150 Km/h", "170 Km/h", "130 Km/h", "140 Km/h", "130 Km/h", "120 Km/h", "150 Km/h" };
    int price[10] = { 250, 400, 400, 500, 900, 150, 430, 230, 200 };
    int year[10] = { 2014, 2012, 2013, 2017, 2020, 2009, 2012, 2010, 2014 };
    int size = 9;
};

struct CustomerInfo {
    string name;
    string icnum;
    string phone;
    int rentDate[3]; // year, month, day
    int days;
    int payment;
    int balance;
};

Cars car;
CustomerInfo customer;

void shiftArrays(int index) {
    for (int i = index; i < car.size - 1; ++i) {
        car.mark[i] = car.mark[i + 1];
        car.model[i] = car.model[i + 1];
        car.color[i] = car.color[i + 1];
        car.maxSpeed[i] = car.maxSpeed[i + 1];
        car.price[i] = car.price[i + 1];
        car.year[i] = car.year[i + 1];
    }
    car.size--;
}

bool validateICNumber(const string& ic) {
    if (ic.length() != 12) {
        cerr << "\t\t\t\tInvalid IC number! It must have 12 digits.\n";
        return false;
    }

    for (char c : ic) {
        if (!isdigit(c)) {
            cerr << "\t\t\t\tInvalid IC number! It must contain only digits.\n";
            return false;
        }
    }

    string birthYearStr = ic.substr(0, 2);
    string birthMonthStr = ic.substr(2, 2);
    string birthDayStr = ic.substr(4, 2);

    int birthYear = stoi(birthYearStr);
    int birthMonth = stoi(birthMonthStr);
    int birthDay = stoi(birthDayStr);

    birthYear += (birthYear <= 23) ? 2000 : 1900;

    time_t now = time(0);
    tm* local_time = localtime(&now);
    int currentYear = local_time->tm_year + 1900;
    int currentMonth = local_time->tm_mon + 1;
    int currentDay = local_time->tm_mday;

    int age = currentYear - birthYear;
    if (birthMonth > currentMonth || (birthMonth == currentMonth && birthDay > currentDay)) {
        age--;
    }

    if (age < 18) {
        cerr << "\t\t\t\tYou must be at least 18 years old to rent a car. Your age is: " << age << ".\n";
        return false;
    }

    return true;
}

bool isLeapYear(int year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

int daysSince1900(int year, int month, int day) {
    int days = 0;
    for (int i = 1900; i < year; i++) {
        days += (isLeapYear(i) ? 366 : 365);
    }

    int daysInMonth[] = { 31, (isLeapYear(year) ? 29 : 28), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    for (int i = 0; i < month - 1; i++) {
        days += daysInMonth[i];
    }

    days += day;
    return days;
}

bool validatePhoneNumber(const string& phone) {
    if (phone.empty() || (phone.length() != 10 && phone.length() != 11)) {
        cerr << "Invalid phone number length.\n";
        return false;
    }
    for (char c : phone) {
        if (!isdigit(c)) return false;
    }
    return true;
}

int parseAmount(const string& amountStr) {
    string numericStr;
    for (char c : amountStr) {
        if (isdigit(c)) numericStr.push_back(c);
    }

    try {
        return stoi(numericStr);
    }
    catch (...) {
        cerr << "Error: Invalid payment value!" << endl;
        return -1;
    }
}

int RetCar() {
    ifstream inputFile("CustomerDetails.txt");
    if (!inputFile) {
        cerr << "Error: Could not open the file!" << endl;
        return -1;
    }

    string line;
    while (getline(inputFile, line)) {
        cout << line << endl;
    }
    cout << "\n=========================== END OF CUSTOMER DETAILS ====================\n";
    inputFile.close();

    int returnYear, returnMonth, returnDay;
    cout << "Enter the return year: "; cin >> returnYear;
    cout << "Enter the return month: "; cin >> returnMonth;
    cout << "Enter the return day: "; cin >> returnDay;

    int rentalYear = 0, rentalMonth = 0, rentalDay = 0;

    inputFile.open("CustomerDetails.txt");
    while (getline(inputFile, line)) {
        if (line.find("Rent Start Date") != string::npos) {
            size_t startPos = line.find(":") + 2;
            string dateStr = line.substr(startPos);

            size_t firstSlash = dateStr.find('/');
            size_t secondSlash = dateStr.find('/', firstSlash + 1);

            try {
                rentalYear = stoi(dateStr.substr(0, firstSlash));
                rentalMonth = stoi(dateStr.substr(firstSlash + 1, secondSlash - firstSlash - 1));
                rentalDay = stoi(dateStr.substr(secondSlash + 1));
            } catch (...) {
                cerr << "Error: Invalid date format in file!" << endl;
                return -1;
            }
        }
    }
    inputFile.close();

    if (rentalYear == 0 && rentalMonth == 0 && rentalDay == 0) {
        cerr << "Error: Rental start date not found in the file!" << endl;
        return -1;
    }

    int rentalStartDays = daysSince1900(rentalYear, rentalMonth, rentalDay);
    int returnDays = daysSince1900(returnYear, returnMonth, returnDay);
    int rentalDuration = returnDays - rentalStartDays;

    int initialRentDuration = 0;
    inputFile.open("CustomerDetails.txt");
    while (getline(inputFile, line)) {
        if (line.find("Rent Duration") != string::npos) {
            size_t startPos = line.find(":") + 2;
            try {
                initialRentDuration = stoi(line.substr(startPos));
            } catch (...) {
                cerr << "Error: Invalid rental duration in file!" << endl;
                return -1;
            }
        }
    }
    inputFile.close();

    int penaltyAmount = 0;
    if (rentalDuration > initialRentDuration) {
        int penaltyDays = rentalDuration - initialRentDuration;
        penaltyAmount = penaltyDays * 100;
        cout << "You have exceeded your rental period by " << penaltyDays << " days. Penalty: RM " << penaltyAmount << endl;
    } else if (rentalDuration < initialRentDuration) {
        int unusedDays = initialRentDuration - rentalDuration;
        int refundAmount = unusedDays * 100;
        cout << "You have unused " << unusedDays << " days. Refund: RM " << refundAmount << endl;
    } else {
        cout << "\nThank you for using our services!\n";
    }

    int priorBalance = 0;
    inputFile.open("CustomerDetails.txt");
    while (getline(inputFile, line)) {
        if (line.find("Balance") != string::npos) {
            size_t startPos = line.find(":") + 2;
            priorBalance = parseAmount(line.substr(startPos));
            if (priorBalance == -1) return -1;
        }
    }
    inputFile.close();

    if (penaltyAmount > 0) {
        int remainingPenalty = penaltyAmount - priorBalance;
        int totalPayment = 0;

        if (remainingPenalty > 0) {
            cout << "Remaining penalty after balance RM" << priorBalance << ": RM" << remainingPenalty << endl;
            while (totalPayment < remainingPenalty) {
                int payment;
                cout << "Please pay RM" << (remainingPenalty - totalPayment) << ": RM ";
                cin >> payment;
                if (payment > 0) totalPayment += payment;
            }
        } else {
            cout << "Your balance covers the penalty. Refund: RM" << abs(remainingPenalty) << endl;
        }

        cout << "\nYour penalty has been fully paid. Thank you!\n";
    }

    return 0;
}

void user_input(int theChoice) {
    ofstream outputFile("CustomerDetails.txt", ios::app);
    if (!outputFile) {
        cerr << "File could not be opened\n";
        exit(1);
    }

    cout << "\nEnter your name: ";
    cin.ignore();
    getline(cin, customer.name);

    bool validIC = false;
    while (!validIC) {
        cout << "Enter your IC number (12 digits, no '-') : ";
        cin >> customer.icnum;
        validIC = validateICNumber(customer.icnum);
    }

    bool validPhone = false;
    while (!validPhone) {
        cout << "Enter your phone number: ";
        cin >> customer.phone;
        validPhone = validatePhoneNumber(customer.phone);
    }

    cout << "Enter rental start year: ";
    cin >> customer.rentDate[0];
    do {
        cout << "Enter rental start month (1-12): ";
        cin >> customer.rentDate[1];
    } while (customer.rentDate[1] < 1 || customer.rentDate[1] > 12);

    do {
        cout << "Enter rental start day (1-31): ";
        cin >> customer.rentDate[2];
    } while (customer.rentDate[2] < 1 || customer.rentDate[2] > 31);

    cout << "How many days you want to rent: ";
    cin >> customer.days;

    int rp = customer.days * car.price[theChoice - 1];
    cout << "Total Price : RM" << rp << endl;

    int totalPayment = 0;
    while (totalPayment < rp) {
        cout << "Payment amount: RM ";
        cin >> customer.payment;
        totalPayment += customer.payment;

        if (totalPayment < rp) {
            cout << "You still owe RM" << rp - totalPayment << endl;
        }
    }

    customer.balance = totalPayment - rp;

    outputFile << "\n=========================== CUSTOMER DETAILS ============================\n"
               << "Name : " << customer.name << "\n"
               << "IC : " << customer.icnum << "\n"
               << "Phone : " << customer.phone << "\n"
               << "Rent Duration : " << customer.days << "\n"
               << "Payment : RM" << totalPayment << "\n"
               << "Balance : RM" << customer.balance << "\n"
               << "Rent Start Date : " << customer.rentDate[0] << "/" << customer.rentDate[1] << "/" << customer.rentDate[2] << "\n";
    outputFile.close();
    cout << "\nRenting process complete!\n";
}

void Details(int theChoice) {
    cout << "\nYou selected: " << car.mark[theChoice - 1] << "\n"
         << "Model: " << car.model[theChoice - 1] << "\n"
         << "Color: " << car.color[theChoice - 1] << "\n"
         << "Max Speed: " << car.maxSpeed[theChoice - 1] << "\n"
         << "Price: RM " << car.price[theChoice - 1] << "\n";
}

void Menu() {
    for (int i = 0; i < car.size; ++i) {
        cout << "Enter " << i + 1 << " to select - " << car.mark[i] << "\n";
    }
}

void rent() {
    string decide = "yes";
    while (decide != "exit") {
        Menu();
        int theChoice;
        cout << "Pick a car: ";
        cin >> theChoice;

        while (cin.fail() || theChoice < 1 || theChoice > car.size) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice. Try again: ";
            cin >> theChoice;
        }

        Details(theChoice);
        cout << "Are you sure to rent this car? (yes / no / exit): ";
        cin >> decide;

        if (decide == "yes") {
            user_input(theChoice);
            shiftArrays(theChoice - 1);
            cout << "Do you want to rent another car? (yes / no): ";
            cin >> decide;
            if (decide == "no") return;
        } else if (decide == "no") {
            continue;
        } else if (decide == "exit") {
            cout << "Goodbye!\n";
            exit(0);
        }
    }
}

int main() {
    int decide;
    while (true) {
        cout << "\nMajestic Motors Car Rental Sdn. Bhd.\n"
             << "1. Rent\n"
             << "2. Return\n"
             << "3. Exit\n"
             << "Choose your service: ";
        cin >> decide;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cerr << "Invalid input. Try again.\n";
            continue;
        }

        switch (decide) {
            case 1:
                rent();
                break;
            case 2:
                RetCar();
                break;
            case 3:
                cout << "Thank you for using our service!\n";
                return 0;
            default:
                cerr << "Invalid choice. Try again.\n";
        }
    }
    return 0;
}
