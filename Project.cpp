#include <iostream>
#include <iomanip>
#include <string>
#include <stack>
#include <queue>
#include <unordered_map>
using namespace std;

// Contact node structure
struct Contact {
    string name;
    string phoneNumber;
    Contact* prev;
    Contact* next;

    Contact(string n, string p) : name(n), phoneNumber(p), prev(nullptr), next(nullptr) {}
};

// Undo Stack
stack<string> undoStack;

// Priority Queue (min-heap) for most accessed contacts
struct ContactPriorityQueue {
    unordered_map<string, int> accessCount; // Maps contact names to their access count

    void incrementAccess(string name) {
        accessCount[name]++;
    }

    void displayPriorityQueue() {
        cout << "\nMost Accessed Contacts:\n";
        for (auto& pair : accessCount) {
            cout << "Name: " << pair.first << ", Access Count: " << pair.second << endl;
        }
    }
};

// Function to set text color (ANSI escape codes)
void setColor(string color) {
    if (color == "yellow")
        cout << "\033[1;33m"; // Yellow
    else if (color == "cyan")
        cout << "\033[1;36m"; // Cyan
    else if (color == "green")
        cout << "\033[1;32m"; // Green
    else
        cout << "\033[0m"; // Reset to default
}

// Function to display the menu
void displayMenu() {
    setColor("cyan");
    cout << "\n" << string(50, '*') << "\n";
    cout << setw(30) << " *** PHONE DIRECTORY MENU *** " << "\n";
    cout << string(50, '*') << "\n";
    setColor("yellow");
    cout << " 1 - View Contacts\n";
    cout << " 2 - Search For a Contact\n";
    cout << " 3 - Add Contact\n";
    cout << " 4 - Edit Contact\n";
    cout << " 5 - Delete Contact\n";
    cout << " 6 - Sort Contacts\n";
    cout << " 7 - Undo Last Action\n";
    cout << " 8 - View Most Accessed Contacts\n";
    cout << " 0 - Exit\n";
    setColor("green");
    cout << "\nHow can I assist you? (0-8): ";
    setColor("reset");
}

// Function to add a contact to the directory (sorted order)
void addContact(Contact*& head, string name, string phone, ContactPriorityQueue& pq) {
    Contact* newContact = new Contact(name, phone);

    // Insert at the beginning if the list is empty or the new contact comes first
    if (!head || head->name > name) {
        newContact->next = head;
        if (head) head->prev = newContact;
        head = newContact;
        cout << "\n*** Contact added successfully! ***\n";
        undoStack.push("Add " + name); // Record action for undo
        return;
    }

    // Traverse to find the correct position for the new contact
    Contact* current = head;
    while (current->next && current->next->name < name) {
        current = current->next;
    }

    // Insert the new contact in the correct position
    newContact->next = current->next;
    if (current->next) current->next->prev = newContact;
    current->next = newContact;
    newContact->prev = current;

    cout << "\n*** Contact added successfully! ***\n";
    undoStack.push("Add " + name); // Record action for undo
    pq.incrementAccess(name); // Increment access count for priority queue
}

// Function to display all contacts
void displayContacts(Contact* head) {
    if (!head) {
        cout << "\nNo contacts to display.\n";
        return;
    }
    Contact* current = head;
    cout << "\n--- Contact List ---\n";
    while (current) {
        cout << "Name: " << current->name << ", Phone: " << current->phoneNumber << endl;
        current = current->next;
    }
}

// Function to search for contacts by name or phone number
void searchContact(Contact* head, string query, ContactPriorityQueue& pq) {
    if (!head) {
        cout << "\nNo contacts available.\n";
        return;
    }
    Contact* current = head;
    bool found = false;
    cout << "Search results for \"" << query << "\":\n";
    while (current) {
        // Check if the query matches the name or phone number
        if (current->name.find(query) != string::npos || current->phoneNumber.find(query) != string::npos) {
            cout << "Name: " << current->name << ", Phone: " << current->phoneNumber << endl;
            found = true;
            pq.incrementAccess(current->name); // Increment access count for priority queue
        }
        current = current->next;
    }
    if (!found) {
        cout << "\nNo matching contacts found.\n";
    }
}

// Function to delete a contact by name
void deleteContact(Contact*& head, string name, ContactPriorityQueue& pq) {
    if (!head) {
        cout << "\nThe contact list is empty.\n";
        return;
    }
    Contact* current = head;
    while (current && current->name != name) {
        current = current->next;
    }
    if (!current) {
        cout << "\nContact not found!\n";
        return;
    }

    // Adjust pointers to delete the contact
    if (current->prev) current->prev->next = current->next;
    if (current->next) current->next->prev = current->prev;
    if (current == head) head = current->next;
    delete current;
    cout << "\n*** Contact deleted successfully! ***\n";
    undoStack.push("Delete " + name); // Record action for undo
    pq.incrementAccess(name); // Increment access count for priority queue
}

// Function to update an existing contact
void updateContact(Contact* head, string name) {
    if (!head) {
        cout << "\nNo contacts available.\n";
        return;
    }
    Contact* current = head;
    while (current && current->name != name) {
        current = current->next;
    }
    if (!current) {
        cout << "\nContact not found!\n";
        return;
    }
    string newName, newPhone;
    cout << "\nEnter new name: ";
    getline(cin, newName);
    cout << "Enter new phone number: ";
    getline(cin, newPhone);
    current->name = newName;
    current->phoneNumber = newPhone;
    cout << "\n*** Contact updated successfully! ***\n";
}

// Function to sort contacts alphabetically using Bubble Sort
void sortContacts(Contact*& head) {
    if (!head || !head->next) return;

    bool swapped;
    do {
        swapped = false;
        Contact* current = head;
        while (current->next) {
            if (current->name > current->next->name) {
                // Swap the contact details
                swap(current->name, current->next->name);
                swap(current->phoneNumber, current->next->phoneNumber);
                swapped = true;
            }
            current = current->next;
        }
    } while (swapped);
    cout << "\n*** Contacts sorted successfully! ***\n";
}

// Undo the last action
void undoAction(Contact*& head, ContactPriorityQueue& pq) {
    if (undoStack.empty()) {
        cout << "\nNo actions to undo.\n";
        return;
    }

    string lastAction = undoStack.top();
    undoStack.pop();
    if (lastAction.find("Add") != string::npos) {
        string name = lastAction.substr(4); // Extract the name of the contact
        deleteContact(head, name, pq);
    }
    else if (lastAction.find("Delete") != string::npos) {
        string name = lastAction.substr(7); // Extract the name of the contact
        // Code to restore the deleted contact can be implemented here
        cout << "\nUndo feature for 'Delete' is not implemented fully.\n";
    }
}

// Main function
int main() {
    Contact* head = nullptr;
    ContactPriorityQueue pq;
    int choice;
    string name, phone;

    do {
        // Display the menu
        displayMenu();

        // Get the user input
        cin >> choice;
        cin.ignore();  // To handle the newline character

        // Process the choice
        switch (choice) {
        case 1:
            displayContacts(head);
            break;
        case 2:
            cout << "\nEnter search query: ";
            getline(cin, name);
            searchContact(head, name, pq);
            break;
        case 3:
            cout << "\nEnter name: ";
            getline(cin, name);
            cout << "Enter phone number: ";
            getline(cin, phone);
            addContact(head, name, phone, pq);
            break;
        case 4:
            cout << "\nEnter the name of the contact to update: ";
            getline(cin, name);
            updateContact(head, name);
            break;
        case 5:
            cout << "\nEnter the name of the contact to delete: ";
            getline(cin, name);
            deleteContact(head, name, pq);
            break;
        case 6:
            sortContacts(head);
            break;
        case 7:
            undoAction(head, pq);
            break;
        case 8:
            pq.displayPriorityQueue();
            break;
        case 0:
            cout << "\nExiting the program.\n";
            break;
        default:
            cout << "\nInvalid option. Please try again.\n";
        }
    } while (choice != 0);

    return 0;
}
