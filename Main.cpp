//v.1.0.0 -- Credit: https://github.com/KhoabuHtiG
//This is just a beta project! Used for practicing so there is a lot of things that's not good. Thank you!
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include <ctime>
#include <chrono>
namespace fs = std::filesystem;
static fs::path main_direc = "./ATM_Data";

typedef struct {
    int pin;
    int balance;
    std::string username;
    std::string password;
} USER_DATA;

std::string requirePassword(std::string password) {
    if (!(password.size() >= 8)) {
        std::cout << "Password must be 8 characters least.";
        return "";
    }
    return password;
}

int requirePin(int pin) {
    if (!(pin >= 1000 && pin <= 9999)) {
        std::cout << "PIN must be 4 numbers least.";
        return -1;
    }
    return pin;
}

std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));

    return std::string(buf);
}

void save_data(std::string username, std::string password, int balance, int pin) {
    if (!(fs::exists(main_direc))) {
        fs::create_directory(main_direc);
    }
    fs::path userfolder = main_direc / ("Userdata_" + username);
    if (!(fs::exists(userfolder))) {
        fs::create_directory(userfolder);
    }

    try {
        fs::path userdfile = userfolder / ("UserData_" + username + ".txt");
        std::ofstream userdata(userdfile);

        if (userdata.is_open()) {
            userdata << username << '\n';
            userdata << password << '\n';
            userdata << balance << '\n';
            userdata << pin << '\n';
            userdata.close();
        }
    } catch(fs::filesystem_error &e) {
        std::cout << "Error found: " << e.what();
    }
};

bool get_data(std::string &username, std::string &password, int &balance, int &pin) {
    fs::path userfolder = main_direc / ("Userdata_" + username);
    fs::path userdfile = userfolder / ("UserData_" + username + ".txt");
    fs::path user_translogfile = userfolder / ("User_TransLog_" + username + ".txt");
    std::ifstream userdata(userdfile);
    if (userdata.is_open()) {
        std::string fileUser;
        userdata >> fileUser;
        userdata >> password;
        userdata >> balance;
        userdata >> pin;
        userdata.close();
        return (fileUser == username);
    }
    return false;
};

void translog(std::string &username, int amount, std::string trans_type, std::string target, std::string target_name, int balance) {
    fs::path userfolder = main_direc / ("Userdata_" + username);

    try {
        fs::path translog_file = userfolder / ("UserTranslog_" + username + ".txt");
        std::ofstream translog_data(translog_file, std::ios::app);

        if (translog_data.is_open()) {
            if (trans_type == "Transfer") {
                translog_data << "[" << getTimestamp() << "] " << "Transfer " << target << " " << target_name 
                    << ": -" << amount << "$ | Balance: " << balance << "$\n";
            }
            translog_data.close();
        }
    } catch (fs::filesystem_error &e) {
        std::cout << "Error found: " << e.what();
    }
}

void withdrawlog(std::string &username, int &amount, std::string &trans_type, int &balance) {
    fs::path userfolder = main_direc / ("Userdata_" + username);

    try {
        fs::path translog_file = userfolder / ("UserTranslog_" + username + ".txt");
        std::ofstream translog_data(translog_file, std::ios::app);

        if (translog_data.is_open()) {
            if (trans_type == "Withdraw") {
                translog_data << "[" << getTimestamp() << "] " 
                    << "Withdrawed: " << ": -" << amount << "$ | Balance: " << balance << "$\n";
            }
            translog_data.close();
        }
    } catch (fs::filesystem_error &e) {
        std::cout << "Error found: " << e.what();
    }
}

void depositlog(std::string &username, int &amount, std::string &trans_type, int &balance) {
    fs::path userfolder = main_direc / ("Userdata_" + username);

    try {
        fs::path translog_file = userfolder / ("UserTranslog_" + username + ".txt");
        std::ofstream translog_data(translog_file, std::ios::app);

        if (translog_data.is_open()) {
            if (trans_type == "Deposit") {
                translog_data << "[" << getTimestamp() << "] " 
                    << "Deposited: " << ": -" << amount << "$ | Balance: " << balance << "$\n";
            }
            translog_data.close();
        }
    } catch (fs::filesystem_error &e) {
        std::cout << "Error found: " << e.what();
    }
}


bool viewtranslog(std::string &username) {
    fs::path userdatafile = main_direc / ("UserData_" + username);

    try {
        fs::path translog_file = userdatafile / ("UserTranslog_" + username + ".txt");
        std::ifstream translog_data(translog_file, std::ios::in);

        if (translog_data.is_open()) {
            std::cout << "===TRANSACTION HISTORY===\n";
            std::string line;
            int count = 0;

            while (std::getline(translog_data, line) && count < 10) {
                std::cout << line << '\n';
                count++;
                return true;
            }

            if (count == 0) {
                std::cout << "No transaction history found. ";
                return false;
            }
            translog_data.close();
        }

        std::cout << "No transaction history found. ";
    } catch (fs::filesystem_error &e) {
        std::cout << "Error found: " << e.what();
        return false;
    } 
};

void transaction(std::string &username, int &pin, int &current_balance, std::string &password) {
    int trans_pin, trans_balance, target_balance, target_pin;
    std::string target_name, target_pass, type = "Transfer", t = "to";

    std::cout << "Enter the recipient's username: ";
    std::cin >> target_name;

    fs::path target_folder = main_direc / ("Userdata_" + target_name);
    fs::path target_datafile = target_folder / ("UserData_" + target_name + ".txt");
    std::ifstream targetuserdata(target_datafile);

    if (!(targetuserdata.is_open())) {
        std::cout << "No account found with this name. ";
        return;
    }

    targetuserdata >> target_name;
    targetuserdata >> target_pass;
    targetuserdata >> target_balance;
    targetuserdata >> target_pin;
    std::cout << "Enter your amount for transaction: ";
    std::cin >> trans_balance;

    if (trans_balance <= current_balance) {
        std::cout << "Enter your PIN for continue: ";
        std::cin >> trans_pin;

        if (!(trans_pin == pin)) {
            std::cout << "Inccorect PIN\n";
            return;
        } 

        current_balance -= trans_balance;
        target_balance += trans_balance;

        std::cout << "Transacted successfully\n";
        std::cout << "You have transacted " << trans_balance << "$ to " << target_name << '\n';
        std::cout << "Remaining balance: " << current_balance << "$\n";

        translog(username, trans_balance, type, t, target_name, current_balance);
        save_data(username, password, current_balance, pin);
        save_data(target_name, target_pass, target_balance, target_pin);
        targetuserdata.close();
        return;
    }

    std::cout << "Insufficient funds\n";
    return;
};

bool login(std::string &password) {
    std::string comfirmPassword;
    int tries = 0, wait_time = 30;

    while (true) {
        std::cout << "Enter your password: ";
        std::cin >> comfirmPassword;

        if (!(comfirmPassword == password)) {
            std::cout << "Incorrect password.\n";
            tries++;

            if (tries >= 3) {
                std::cout << "Too many failed attempts. Please wait before retrying.\n";    
                for (int i = wait_time; i > 0; i--) {
                    std::cout << "\rRetry available in: " << i << " seconds..";
                    std::cout.flush();
                    Sleep(1000);
                }

                std::cout << "\nYou may try again now. ";
                tries = 2, wait_time *= 2;
            }
        } else {
            std::cout << "Login successful!\n";
            return true;
        }
    }
};

void new_account() {
    std::string username, password;
    int balance = 10000, pin;

    while (true) {
        std::cout << "Set a username: ";
        std::cin >> username;

        while (true) {
            std::cout << "Set a password (min 8 characters): ";
            std::cin >> password;

            std::string checkPassword = requirePassword(password);
            if (checkPassword == "") {
                std::cout << "Password must be 8 characters least.";
                break;
            }

            std::cout << "Set a 4-digit PIN (for withdrawals, deposits, and security): ";
            std::cin >> pin;

            int checkPin = requirePin(pin);
            if (checkPin == -1) {
                std::cout << "PIN must be 4 numbers least.";
                break;
            } 

            save_data(username, password, balance, pin);
            break;
        }
        return;
    }
    return;
};

void withdraw(int &balance, std::string username, std::string pass, int pin) {
    std::string withdrawType = "Withdraw";
    int withdrawMoney, comfirmPin;

    std::cout << "Enter your PIN: ";
    std::cin >> comfirmPin;

    if (comfirmPin != pin) {
        std::cout << "Inccorect PIN. ";
        return;
    }

    std::cout << "Current balance: " << balance << "$\n";
    std::cout << "Enter the amount you want to withdraw: ";
    std::cin >> withdrawMoney;

    if (withdrawMoney > balance) {
        std::cout << "Insufficient funds";
        return;
    }

    balance -= withdrawMoney;

    std::cout << "Withdrawed successfully\n";
    std::cout << "You withdrawed: " << withdrawMoney << "$\n";
    std::cout << "Current balance: " << balance << "$\n";

    save_data(username, pass, balance, pin);
    withdrawlog(username, withdrawMoney, withdrawType, balance);
}

void deposit(int &balance, std::string username, std::string pass, int pin) {
    std::string depositType = "Deposit";
    int depositMoney, comfirmPin;

    std::cout << "Enter your PIN: ";
    std::cin >> comfirmPin;

    if (comfirmPin != pin) {
        std::cout << "Inccorect PIN. ";
        return;
    }

    std::cout << "Current balance: " << balance << "$\n";
    std::cout << "Enter the amount you want to deposit: ";
    std::cin >> depositMoney;

    if (depositMoney > balance) {
        std::cout << "Insufficient funds";
        return;
    }

    balance -= depositMoney;

    std::cout << "Deposited successfully\n";
    std::cout << "You deposited: " << depositMoney << "$\n";
    std::cout << "Current balance: " << balance << "$\n";

    save_data(username, pass, balance, pin);
    depositlog(username, depositMoney, depositType, balance);
}

void changepass(std::string &username, std::string password, int balance, int pin) {
    std::string newPassword;

    std::cout << "Please type in your new password (min 8 characters): ";
    std::cin >> newPassword;

    std::string checkPassword = requirePassword(newPassword);
    if (checkPassword != "") {
        std::cout << "Changed password successfully!\n";
        save_data(username, password, balance, pin);
    } return;

    std::cout << "Password length must be 8 characters least.\n";
    return;
}

bool removeAccount(std::string &username) {
    fs::path userfolder = main_direc / ("Userdata_" + username);
    
    try {
        bool removed = fs::remove_all(userfolder);
        if (!(removed)) {
            std::cout << "Failed to remove account. Please try again later.\n";
            return false;
        }
        std::cout << "Removed successfully\n";
        return true;
    } catch(fs::filesystem_error &e) {
        std::cout << "Error found: " << e.what() << '\n';
        return false;
    }
};

class your_ATM {
private:
    USER_DATA userdata;
public:
    your_ATM(int balance, std::string username, int pin, std::string password) {
        userdata.balance = balance;
        userdata.username = username;
        userdata.pin = pin;
        userdata.password = password;
    }

    auto Main() {
        system("cls");
        char option;
        int comfirmPin;

        std::vector<std::string> menu = {
            "=====================================",
            "               ATM MENU              ",
            "=====================================",
            " [1] Check Balance", " [2] Transfer Funds", " [3] Withdraw Money", " [4] Deposit Money",
            " [5] Change Password", " [6] View Transaction History, ", " [7] Delete/remove account"
        };

        std::cout << userdata.username << " account\n";
        for (std::string i : menu) {std::cout << i << '\n';}

        while (true) {
            std::cout << "Type in your options(1-7), m to show the menu again, q to log out: ";
            std::cin >> option;

            if (option == 'q' || option == 'Q') {std::cout << "Data saved\n"; break;}
            
            if (option == 'm' || option == 'M') {
                for (std::string i : menu) {
                    std::cout << i << '\n';
                }
            }

            switch(option) {
                case('1'):
                    std::cout << "Current balance: " << userdata.balance << "$\n";
                    break;

                case('2'):
                    transaction(userdata.username, userdata.pin, userdata.balance, userdata.password);
                    break;

                case('3'):
                    std::cout << "Enter your PIN: ";
                    std::cin >> comfirmPin;

                    if (comfirmPin != userdata.pin) {
                        std::cout << "Inccorect PIN. ";
                        break;
                    }

                    withdraw(userdata.balance, userdata.username, userdata.password, userdata.pin);
                    break;

                case('4'):
                    deposit(userdata.balance, userdata.username, userdata.password, userdata.pin);
                    break;

                case('5'):
                    std::cout << "Please type in your PIN to do this action:";
                    std::cin >> comfirmPin;

                    if (comfirmPin == userdata.pin) {
                        changepass(userdata.username, userdata.password, userdata.balance, userdata.pin);
                        break;
                    }

                    std::cout << "Inccorect PIN\n";
                    break;

                case('6'):
                    viewtranslog(userdata.username);
                    break;

                case('7'):
                    bool removed = removeAccount(userdata.username);
                    if (removed) {
                        return;
                    }

            }
        }
        return;
    }
};

void loginMenu() {
    USER_DATA userdata;
    char option;

    system("cls");  
    std::cout << "=== Welcome to ATM System ===" << std::endl;
 
    while (true) {
        std::cout << "|| Log in: l || Sign up: y || Quit: q" << std::endl;
        std::cout << "Choose: ";
        std::cin >> option;

        if (option == 'q' || option == 'Q') {
            std::cout << "Goodbye!";
            break;
        }

        switch (option) {
            case 'l':
            case 'L': {
                std::cout << "Enter your username: ";
                std::cin >> userdata.username;

                if (!(get_data(userdata.username, userdata.password, userdata.balance, userdata.pin))) {
                    std::cout << "No account found with this name\n";
                    break;
                } else {
                    bool logged = login(userdata.password);
                    if (logged == true) {
                        your_ATM myClass(userdata.balance, userdata.username, userdata.pin, userdata.password);
                        myClass.Main();
                        return;
                    }
                }
            }

            case 'y':
            case 'Y': 
                std::cout << "Creating a new account for you . . .\n";
                new_account();
                return;

            default:
                std::cout << "Invalid option\n";
                break;
        }
    }
    return;
}

int main() {
    loginMenu();
    return 0;
}
