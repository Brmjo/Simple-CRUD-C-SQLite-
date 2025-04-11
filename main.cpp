#include <iostream>
#include <sqlite3.h>
#include <string>
#include <iostream>
#include <cstdlib> // for system()

// ANSI color codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define YELLOW  "\033[33m"
#define BOLD    "\033[1m"

class Database {
	const char* databasePath;
	
public: 
	Database(const char* path) {
		this->databasePath = path;
	}
	
	int insertData(std::string name, std::string quantity, std::string price) {
		sqlite3* db;
		sqlite3_stmt* stmt;
		int exit = sqlite3_open(this->databasePath, &db);
		
		if(exit) {
			std::cout << "Error opening database: " << sqlite3_errmsg(db) << '\n';
			return exit;
		}
		
		const char* query = "INSERT INTO products (Name, Quantity, Price) VALUES (?, ?, ?)";
		
		exit = sqlite3_prepare_v2(db, query, -1, &stmt, 0);
		
		if(exit != SQLITE_OK) {
			std::cout << "Error preparing statement: " << sqlite3_errmsg(db) << '\n';
			sqlite3_close(db);
			return exit;
		}
		
		sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, quantity.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 3, price.c_str(), -1, SQLITE_STATIC);
		
		exit = sqlite3_step(stmt);
		
		if(exit != SQLITE_DONE) {
			std::cout << "Error inserting data: " << sqlite3_errmsg(db) << '\n';
		}else {
			std::cout << "Successfully inserted data.\n";
		}
		
		sqlite3_finalize(stmt);
		sqlite3_close(db);
	}
	
	static int callback(void* notUsed, int argc, char** argv, char** colName) {
		for(int i = 0; i < argc; i++) {
			std::cout << colName[i] << ": " << (argv[i] ? argv[i] : "NULL") << '\n';
		}
		return 0;
	}
	
	int getAllData() {
		sqlite3* db;
		char* error;
		int exit = sqlite3_open(this->databasePath, &db);
		
		if(exit) {
			std::cout << "Error opening database: " << sqlite3_errmsg(db) << '\n';
			return exit;
		}
		
		const char* query = "SELECT * FROM products;";
		
		exit = sqlite3_exec(db, query, callback, nullptr, &error);
		
		if(exit != SQLITE_OK) {
			std::cout << "Error retrieving data: " << sqlite3_errmsg(db) << '\n';
			sqlite3_close(db);
			return exit;
		}
		
		sqlite3_close(db);
		return 0;
	}
	
	int updateData(std::string column, std::string newValue, int id) {
		sqlite3* db;
		sqlite3_stmt* stmt;
		
		int exit = sqlite3_open(this->databasePath, &db);
		
		if(exit != SQLITE_OK) {
			std::cout << "Error opening database: " << sqlite3_errmsg(db) << '\n';
			return exit;
		} 
		
		std::string sql = "UPDATE products SET " + column + "  = ? WHERE id = ?;"; 
		
		sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
		
		sqlite3_bind_text(stmt, 1, newValue.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 2, id);
		
		exit = sqlite3_step(stmt);
		
		if(exit != SQLITE_DONE) {
			std::cout << "Error updating data: " << sqlite3_errmsg(db) << '\n';
			return exit;
		}else {
			std::cout << "Successfully updated data\n";
		}
		
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return 0;
	}
	
	int deleteData(int id) {
		sqlite3* db;
		sqlite3_stmt* stmt;
		
		int exit = sqlite3_open(this->databasePath, &db);
		
		if(exit != SQLITE_OK) {
			std::cout << "Error opening database: " << sqlite3_errmsg(db) << '\n';
			return exit;
		}
		
		std::string sql = "DELETE FROM products WHERE id = ?";
		
		sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
		
		sqlite3_bind_int(stmt, 1, id);
		
		exit = sqlite3_step(stmt);
		
		if(exit != SQLITE_DONE) {
			std::cout << "Error deleting data: " << sqlite3_errmsg(db) << '\n';
			return exit;
		}else {
			std::cout << "Successfully deleted data\n";
		}
		
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return 0;
	}
	
};

int main() {
	
	Database db("/storage/emulated/0/C++ Projects/simple-inventory-system/item-database.db");
	
	int choice;
    do {
        system("clear"); // or "cls" on Windows

        std::cout << CYAN << BOLD << "===== Simple Inventory System =====\n" << RESET;
        std::cout << YELLOW << "1." << RESET << " Insert product\n";
        std::cout << YELLOW << "2." << RESET << " View all products\n";
        std::cout << YELLOW << "3." << RESET << " Update a product\n";
        std::cout << YELLOW << "4." << RESET << " Delete a product\n";
        std::cout << YELLOW << "0." << RESET << " Exit\n";
        std::cout << BOLD << "Choose an option: " << RESET;
        std::cin >> choice;
        std::cin.ignore();

        system("clear");

        switch(choice) {
            case 1: {
                std::string name, quantity, price;
                std::cout << "Enter product name: ";
                std::getline(std::cin, name);
                std::cout << "Enter quantity: ";
                std::getline(std::cin, quantity);
                std::cout << "Enter price: ";
                std::getline(std::cin, price);
                db.insertData(name, quantity, price);
                break;
            }
            case 2:
                db.getAllData();
                break;
            case 3: {
                int id;
                std::string column, newValue;
                std::cout << "Enter product ID to update: ";
                std::cin >> id;
                std::cin.ignore();
                std::cout << "Enter column to update (Name, Quantity, Price): ";
                std::getline(std::cin, column);
                std::cout << "Enter new value: ";
                std::getline(std::cin, newValue);
                db.updateData(column, newValue, id);
                break;
            }
            case 4: {
                int id;
                std::cout << "Enter product ID to delete: ";
                std::cin >> id;
                db.deleteData(id);
                break;
            }
            case 0:
                std::cout << GREEN << "Exiting program...\n" << RESET;
                break;
            default:
                std::cout << RED << "Invalid option. Try again.\n" << RESET;
        }

        if (choice != 0) {
            std::cout << "\nPress Enter to continue...";
            std::cin.get();
        }

    } while (choice != 0);

    return 0;
}