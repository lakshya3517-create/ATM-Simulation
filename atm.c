/*
 * ATM Simulation System (C)
 * Concepts: Switch Case, Loops, Functions
 * Features: PIN login (3 attempts), balance enquiry, cash withdrawal,
 *           deposit, PIN change.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ATTEMPTS      3
#define WITHDRAW_LIMIT    20000.0   /* per transaction */
#define NOTE_MULTIPLE     100       /* ATM dispenses multiples of 100 */
#define DEFAULT_PIN       1234
#define INITIAL_BALANCE   10000.0

/* ---------- input helpers ---------- */

/* Read a whole line and parse an int. Returns 1 on success, 0 on bad input. */
static int read_int(int *out)
{
    char buf[64], *end;
    long v;
    if (!fgets(buf, sizeof buf, stdin)) return 0;
    v = strtol(buf, &end, 10);
    if (end == buf) return 0;
    while (*end == ' ' || *end == '\t') end++;
    if (*end != '\n' && *end != '\0') return 0;
    *out = (int)v;
    return 1;
}

/* Read a positive amount. Returns 1 on success, 0 on bad input. */
static int read_amount(double *out)
{
    char buf[64], *end;
    double v;
    if (!fgets(buf, sizeof buf, stdin)) return 0;
    v = strtod(buf, &end);
    if (end == buf) return 0;
    while (*end == ' ' || *end == '\t') end++;
    if (*end != '\n' && *end != '\0') return 0;
    *out = v;
    return 1;
}

/* Read a 4-digit PIN. Returns 1 on success, 0 on bad input. */
static int read_pin(int *out)
{
    char buf[32];
    int i, len;
    if (!fgets(buf, sizeof buf, stdin)) return 0;
    len = (int)strcspn(buf, "\n");
    if (len != 4) return 0;
    for (i = 0; i < 4; i++)
        if (!isdigit((unsigned char)buf[i])) return 0;
    *out = atoi(buf);
    return 1;
}

/* ---------- ATM operations ---------- */

static int login(int pin)
{
    int attempt, entered;
    for (attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
        printf("Enter 4-digit PIN: ");
        if (read_pin(&entered) && entered == pin) {
            printf("Login successful.\n");
            return 1;
        }
        printf("Wrong PIN. Attempts left: %d\n", MAX_ATTEMPTS - attempt);
    }
    printf("Card blocked. Please contact your bank.\n");
    return 0;
}

static void check_balance(double balance)
{
    printf("Your current balance is: Rs. %.2f\n", balance);
}

static void withdraw_cash(double *balance)
{
    double amount;
    printf("Enter amount to withdraw: ");
    if (!read_amount(&amount) || amount <= 0) {
        printf("Invalid amount.\n");
        return;
    }
    if (((long)amount % NOTE_MULTIPLE) != 0 || amount != (double)(long)amount) {
        printf("Amount must be a multiple of %d.\n", NOTE_MULTIPLE);
        return;
    }
    if (amount > WITHDRAW_LIMIT) {
        printf("Limit exceeded. Maximum per transaction: Rs. %.2f\n", WITHDRAW_LIMIT);
        return;
    }
    if (amount > *balance) {
        printf("Insufficient balance.\n");
        return;
    }
    *balance -= amount;
    printf("Please collect your cash: Rs. %.2f\n", amount);
    printf("Remaining balance: Rs. %.2f\n", *balance);
}

static void deposit_cash(double *balance)
{
    double amount;
    printf("Enter amount to deposit: ");
    if (!read_amount(&amount) || amount <= 0) {
        printf("Invalid amount.\n");
        return;
    }
    *balance += amount;
    printf("Rs. %.2f deposited successfully.\n", amount);
    printf("Updated balance: Rs. %.2f\n", *balance);
}

static void change_pin(int *pin)
{
    int old_pin, new_pin, confirm;
    printf("Enter current PIN: ");
    if (!read_pin(&old_pin) || old_pin != *pin) {
        printf("Incorrect current PIN.\n");
        return;
    }
    printf("Enter new 4-digit PIN: ");
    if (!read_pin(&new_pin)) {
        printf("PIN must be exactly 4 digits.\n");
        return;
    }
    printf("Confirm new PIN: ");
    if (!read_pin(&confirm) || confirm != new_pin) {
        printf("PINs do not match. PIN not changed.\n");
        return;
    }
    *pin = new_pin;
    printf("PIN changed successfully.\n");
}

static void show_menu(void)
{
    printf("\n========== ATM MENU ==========\n");
    printf("1. Balance Enquiry\n");
    printf("2. Cash Withdrawal\n");
    printf("3. Cash Deposit\n");
    printf("4. Change PIN\n");
    printf("5. Exit\n");
    printf("==============================\n");
    printf("Enter your choice: ");
}

int main(void)
{
    int pin = DEFAULT_PIN;
    double balance = INITIAL_BALANCE;
    int choice;

    printf("=== Welcome to the ATM Simulation System ===\n");
    printf("(Demo PIN: %d)\n\n", DEFAULT_PIN);

    if (!login(pin))
        return 1;

    do {
        show_menu();
        if (!read_int(&choice)) {
            if (feof(stdin)) break;
            choice = 0;
        }
        switch (choice) {
            case 1: check_balance(balance);   break;
            case 2: withdraw_cash(&balance);  break;
            case 3: deposit_cash(&balance);   break;
            case 4: change_pin(&pin);         break;
            case 5: printf("Thank you for using our ATM. Goodbye!\n"); break;
            default: printf("Invalid choice. Please enter 1-5.\n");
        }
    } while (choice != 5);

    return 0;
}
