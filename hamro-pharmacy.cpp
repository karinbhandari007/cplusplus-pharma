#include <iostream>
#include <ctime>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <limits>
#include <termios.h>
#include <unistd.h>

using namespace std;

int getch()
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}

class Medicine
{
private:
    int id;
    int quantity;
    int mg;
    char name[50];
    char description[5000];
    int mfgDay, mfgMonth, mfgYear;
    int expDay, expMonth, expYear;
    char info[5000];
    float price;
    int remainingDays, remainingMonths, remainingYears;

public:
    void getData();
    void showData();
    char *getDescription() { return description; }
    int getQuantity() { return quantity; }
    void setQuantity(int q) { quantity = q; }
    int getId() { return id; }
    float getPrice() { return price; }
    char *getName() { return name; }
    string getMfgDate();
    string getExpDate();
    void writeToFile();
    void displayRecords();
    void modifyRecord();
    void displaySpecificRecord(int n);
    void deleteRecord();
    void takeOrder();
    void displayMenu();
    void adminMenu();
};

fstream file;
Medicine medicine;

void Medicine::getData()
{
    cout << "\n\t\t\t Enter the ID of the medicine: ";
    cin >> id;
    cout << "\n\t\t\t Enter the Name of the medicine: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.getline(name, 50);
    cout << "\n\t\t\t Enter the MG: ";
    cin >> mg;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    bool validDate;
    do
    {
        validDate = true;
        cout << "\n\t\t\t Enter the expiration date (DD MM YYYY): ";
        cin >> expDay >> expMonth >> expYear;
        if (expDay < 1 || expDay > 31 || expMonth < 1 || expMonth > 12 || expYear < 1000 || expYear > 9999)
        {
            cout << "Invalid date! Please enter a valid date." << endl;
            validDate = false;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (!validDate);

    do
    {
        validDate = true;
        cout << "\n\t\t\t Enter the manufacturing date (DD MM YYYY): ";
        cin >> mfgDay >> mfgMonth >> mfgYear;
        if (mfgDay < 1 || mfgDay > 31 || mfgMonth < 1 || mfgMonth > 12 || mfgYear < 1000 || mfgYear > 9999)
        {
            cout << "Invalid date! Please enter a valid date." << endl;
            validDate = false;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (!validDate);

    cout << "\n\t\t\t Enter the Quantity: ";
    cin >> quantity;
    cout << "\n\t\t\t Enter the price of the medicine: Rs. ";
    cin >> price;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "\n\t\t\t Enter the description of the medicine: ";
    cin.getline(description, 5000);
}

void Medicine::showData()
{
    cout << "\n\t\t\t The ID of the medicine: " << id;
    cout << "\n\t\t\t The name of the medicine: " << name;
    cout << "\n\t\t\t The MG: " << mg;
    cout << "\n\t\t\t Expiration date: " << expDay << "/" << expMonth << "/" << expYear;
    cout << "\n\t\t\t Manufacturing date: " << mfgDay << "/" << mfgMonth << "/" << mfgYear;
    cout << "\n\t\t\t Quantity: " << quantity;
    cout << "\n\t\t\t Price: Rs. " << price;
    cout << "\n\t\t\t Description of the medicine: " << description;

    time_t now = time(nullptr);
    struct tm expireTime = {0};
    expireTime.tm_year = expYear - 1900;
    expireTime.tm_mon = expMonth - 1;
    expireTime.tm_mday = expDay;

    time_t expireTimeT = mktime(&expireTime);
    double diffSeconds = difftime(expireTimeT, now);
    remainingDays = static_cast<int>(diffSeconds / 86400);
    remainingMonths = remainingDays / 30;
    remainingYears = remainingMonths / 12;
    remainingDays %= 30;
    remainingMonths %= 12;

    cout << "\n\t\t\t Remaining time before expiration: " << remainingYears << " years, " << remainingMonths << " months, " << remainingDays << " days" << endl;
}

string Medicine::getMfgDate()
{
    return to_string(mfgDay) + "/" + to_string(mfgMonth) + "/" + to_string(mfgYear);
}

string Medicine::getExpDate()
{
    return to_string(expDay) + "/" + to_string(expMonth) + "/" + to_string(expYear);
}

void Medicine::writeToFile()
{
    system("clear");
    char choice;
    file.open("pharmacy.txt", ios::out | ios::app);

    do
    {
        cout << "\n Add Medicine" << endl;
        medicine.getData();
        file.write(reinterpret_cast<char *>(&medicine), sizeof(Medicine));
        cout << "\n\n\t\t\t The item has been added successfully !!!" << endl;
        cout << "\n\n\t\t\t Do You Want To Add another item? (y/n): ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    } while (choice == 'y' || choice == 'Y');

    file.close();
}

void Medicine::displayRecords()
{
    system("clear");
    cout << "\n\t\t**** DISPLAY RECORDS ****\n"
         << endl;

    file.open("pharmacy.txt", ios::in);
    if (!file)
    {
        cout << "Error opening file." << endl;
        return;
    }

    file.seekg(0, ios::end);
    int fileSize = file.tellg();
    if (fileSize == 0)
    {
        cout << "File is empty. No records found." << endl;
    }
    else
    {
        file.seekg(0, ios::beg);
        while (file.read(reinterpret_cast<char *>(&medicine), sizeof(Medicine)))
        {
            cout << "\n\t\t\t*******************************\n"
                 << endl;
            medicine.showData();
            cout << "\n\t\t\t*******************************\n"
                 << endl;
        }
    }

    file.close();
}

void Medicine::modifyRecord()
{
    system("clear");
    int id, found = 0;
    cout << "\n\nTo Modify a Record" << endl;
    cout << "\n\t Please Enter the ID of the medicine: ";
    cin >> id;

    file.open("pharmacy.txt", ios::in | ios::out | ios::binary);
    if (!file)
    {
        cout << "Error opening file." << endl;
        return;
    }

    file.seekg(0, ios::beg);
    while (file.read(reinterpret_cast<char *>(&medicine), sizeof(Medicine)))
    {
        if (medicine.getId() == id)
        {
            medicine.showData();
            cout << "\n Please Enter the new Details of the medicine" << endl;
            medicine.getData();
            int pos = -static_cast<int>(sizeof(Medicine));
            file.seekp(pos, ios::cur);
            file.write(reinterpret_cast<char *>(&medicine), sizeof(Medicine));
            cout << "\n\n\t Record Updated" << endl;
            found = 1;
            break;
        }
    }

    file.close();
    if (found == 0)
    {
        cout << "\n\n Record Not Found" << endl;
    }
}

void Medicine::displaySpecificRecord(int id)
{
    system("clear");
    int found = 0;
    file.open("pharmacy.txt", ios::in);
    if (!file)
    {
        cout << "Error opening file." << endl;
        return;
    }

    file.seekg(0, ios::beg);
    while (file.read(reinterpret_cast<char *>(&medicine), sizeof(Medicine)))
    {
        if (medicine.getId() == id)
        {
            medicine.showData();
            found = 1;
            break;
        }
    }

    file.close();
    if (found == 0)
    {
        cout << "\n\n Record does not exist" << endl;
    }
}

void Medicine::deleteRecord()
{
    system("clear");
    int id, found = 0;
    cout << "\n\n\n\t Delete Record" << endl;
    cout << "\n Please Enter The ID of the medicine You Want To Delete: ";
    cin >> id;

    file.open("pharmacy.txt", ios::in | ios::out | ios::binary);
    fstream tempFile("temp.txt", ios::out | ios::binary);

    if (!file || !tempFile)
    {
        cout << "Error opening file(s)." << endl;
        return;
    }

    file.seekg(0, ios::beg);
    while (file.read(reinterpret_cast<char *>(&medicine), sizeof(Medicine)))
    {
        if (medicine.getId() != id)
        {
            tempFile.write(reinterpret_cast<char *>(&medicine), sizeof(Medicine));
        }
        else
        {
            medicine.showData();
            cout << "\n\n\t Record Deleted .." << endl;
            found = 1;
        }
    }

    tempFile.close();
    file.close();

    remove("pharmacy.txt");
    rename("temp.txt", "pharmacy.txt");

    if (found == 0)
    {
        cout << "\n\n\t\t\t Record does not exist" << endl;
    }
}

void Medicine::takeOrder()
{
    system("clear");
    int found;
    time_t currentTime;
    time(&currentTime);

    int orderArr[50], quantities[50], count = 0;
    float amount, total = 0;
    char choice;

    do
    {
        system("clear");
        displayMenu();
        cout << "\n\n\t\t\t*****************************\n";
        cout << "\n\t\t\t Take your order";
        cout << "\n\n\t\t\t*****************************\n";

        file.open("pharmacy.txt", ios::in | ios::out | ios::binary);
        fstream tempFile("temp.txt", ios::out | ios::binary | ios::app);

        if (!file || !tempFile)
        {
            cout << "\n Error in opening file(s)" << endl;
            return;
        }

        file.seekg(0, ios::end);
        int length = file.tellg();
        if (length == 0)
        {
            cout << "File is empty. No records found" << endl;
            getch();
        }
        else
        {
            file.seekg(0, ios::beg);
            cout << "\n\n\t\t\t Enter the ID of the medicine: ";
            cin >> orderArr[count];

            file.read(reinterpret_cast<char *>(&medicine), sizeof(Medicine));
            while (file)
            {
                if (medicine.getId() == orderArr[count])
                {
                    found = found + 1;
                    medicine.showData();
                quantityLabel:
                    cout << "\n\t\t\t Quantity: ";
                    cin >> quantities[count];
                    if (medicine.getQuantity() < quantities[count])
                    {
                        cout << "\n\t\t\t Not enough quantity to sell\n";
                        cout << "\n\t\t\t Please re-enter the quantity";
                        goto quantityLabel;
                    }
                    else
                    {
                        medicine.setQuantity(medicine.getQuantity() - quantities[count]);
                        tempFile.write(reinterpret_cast<char *>(&medicine), sizeof(Medicine));
                        goto jump;
                    }
                }
                tempFile.write(reinterpret_cast<char *>(&medicine), sizeof(Medicine));
            jump:
                file.read(reinterpret_cast<char *>(&medicine), sizeof(Medicine));
            }
        }

        if (found == 0)
        {
            cout << "Item not available." << endl;
            getch();
        }

        file.close();
        tempFile.close();
        remove("pharmacy.txt");
        rename("temp.txt", "pharmacy.txt");

        cout << "\n\n\t\t\t Do You want to order another item? (y/n) \n\t\t\t";
        cin >> choice;
        count++;
    } while (choice == 'Y' || choice == 'y');

    cout << "\n\n Thank you for the order" << endl;

    cout << "\n\n\n\t\t\t**** INVOICE ****\n";
    cout << "\n\t\t\t Item ID\tName\tQuantity\tPrice\tAmount\n";

    for (int x = 0; x < count; x++)
    {
        file.open("pharmacy.txt", ios::in | ios::binary);
        file.read(reinterpret_cast<char *>(&medicine), sizeof(Medicine));
        while (!file.eof())
        {
            if (medicine.getId() == orderArr[x])
            {
                amount = medicine.getPrice() * quantities[x];
                cout << "\n\t\t\t " << orderArr[x] << "\t" << medicine.getName() << "\t" << quantities[x] << "\t\t" << medicine.getPrice() << "\t" << amount;
                total += amount;
            }
            file.read(reinterpret_cast<char *>(&medicine), sizeof(Medicine));
        }
        file.close();
    }

    cout << "\n\n\t\t\t Date: " << ctime(&currentTime);
    cout << "\n\n\t\t\t TOTAL = " << total << endl;
}

void Medicine::displayMenu()
{
    file.open("pharmacy.txt", ios::in | ios::binary);
    if (!file)
    {
        cout << "\n\t File could not open" << endl;
        exit(0);
    }

    cout << "\n\n\t\t MENU\n\n";
    cout << "*********************************************************************************************************************\n";
    cout << "ITEM ID\t\tNAME\t\tMFG DATE\t\tEXP DATE\t\tQUANTITY\t\tPRICE\n";
    cout << "*********************************************************************************************************************\n";

    while (file.read(reinterpret_cast<char *>(&medicine), sizeof(Medicine)))
    {
        cout << medicine.getId() << "\t\t" << medicine.getName() << "\t\t" << medicine.getMfgDate() << "\t\t" << medicine.getExpDate() << "\t\t" << medicine.getQuantity() << "\t\t\t" << medicine.getPrice() << endl;
    }

    file.close();
}

void Medicine::adminMenu()
{
    system("clear");
    int choice;
    cout << "\n\n\t\t Press 1. To Add Medicine" << endl;
    cout << "\n\n\t\t Press 2. To Display Medicine Records" << endl;
    cout << "\n\n\t\t Press 3. To Update Medicine Record" << endl;
    cout << "\n\n\t\t Press 4. To Search Medicine Record" << endl;
    cout << "\n\n\t\t Press 5. To Delete Medicine Record" << endl;
    cout << "\n\n\t\t Enter your choice (1-5): ";
    cin >> choice;

    switch (choice)
    {
    case 1:
        medicine.writeToFile();
        break;
    case 2:
        medicine.displayRecords();
        break;
    case 3:
        medicine.modifyRecord();
        break;
    case 4:
        system("clear");
        int id;
        cout << "\n\n\t Please Enter the ID of the medicine: ";
        cin >> id;
        medicine.displaySpecificRecord(id);
        break;
    case 5:
        medicine.deleteRecord();
        break;
    default:
        cout << "\n\n\t\t Please enter a valid option (1-5)" << endl;
        getch();
        cout << "\a";
        adminMenu();
        break;
    }
}

int main()
{
    char username[10], password[10];
    int attempts = 0;

    do
    {
        system("clear");
        time_t currentTime;
        time(&currentTime);
        cout << ctime(&currentTime);
        cout << "-----------------------------------------------------------------------------------------------------------------------" << endl;
        cout << "------------------------------------------ WELCOME TO PHARMACY ---------------------------------------------------------" << endl;
        cout << "\n\n\t\t\t  \xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\  LOGIN FIRST  \xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\  " << endl;
        cout << " \n\n\t                  ENTER USERNAME: ";
        cin >> username;
        cout << " \n\n\t                  ENTER PASSWORD: ";
        cin >> password;

        if (strcmp(username, "admin") == 0 && strcmp(password, "1234") == 0)
        {
            cout << "  \n\n\n\t\t       WELCOME TO PHARMACY!!!! LOGIN IS SUCCESSFUL" << endl;
            cout << "\n\n\n\t\t\t\t Press any key to continue..." << endl;
            getch();
            break;
        }
        else
        {
            cout << "\n\n\t\t        SORRY !!!!  LOGIN IS UNSUCCESSFUL" << endl;
            attempts++;
            getch();
        }
    } while (attempts <= 2);

    if (attempts > 2)
    {
        cout << "\n\n\t Sorry!! You have entered the wrong username and password for three times!!!" << endl;
        exit(0);
    }

    system("clear");
    int choice;
    do
    {
        system("clear");
        cout << "\n\t\t\t********************************************\n";
        cout << "\n\n\n\t\t\t\t\t MAIN MENU";
        cout << "\n\n\n\t\t\t\t 1. Customer";
        cout << "\n\n\n\t\t\t\t 2. Administrator";
        cout << "\n\n\n\t\t\t\t 3. Exit";
        cout << "\n\t\t\t********************************************\n";
        cout << "\n\n\t\t\t\t Please select your option (1-3): ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            medicine.takeOrder();
            break;
        case 2:
            medicine.adminMenu();
            break;
        case 3:
            exit(0);
        default:
            cout << "Please enter a valid option (1-3)" << endl;
            break;
        }

        cout << "\n\n\t\t\t To Continue with other Options, Enter 1. Else, enter any other number: ";
        cin >> choice;
    } while (choice == 1);

    return 0;
}