#include "header.h"
#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>


const char *RECORDS = "./data/records.txt";
const char *USR= "./data/users.txt";


void clearstr(int n, ...) 
{
    va_list ptr;
    char    *str;
    va_start(ptr, n);
    while(n)
    {
        str = va_arg(ptr, char*);
        for (int i = n; i < 50; i++)
        {  
            str[i] = '\0';
        }
        n--;
    }
    va_end(ptr);
}

int get_id()
{
    FILE    *ptr;
    ptr = open(USR,O_RDONLY);
    char *tmp = calloc(1,1024);
    int     i;
    int     count = 0;

    i = 1;
    i = read(ptr, tmp, 1024);
    if (i == -1)
    {
        return 0;
    }
    close(ptr);
    i = 0;
    while(tmp[i])
    {
        if (tmp[i] == '\n')
        {
            count++;
        }
        i++;
    }
	free(tmp);
    return count;
}


char	*get_name(char *data, int start, int end)
{
	char	*result;
	int		i;
	
	result = calloc(1,50);
	i = 0;
	while (start < end)
	{
		result[i] = data[start];
		start++;
        i++;
	}
   
	return result;

}

bool compare(char *str1, char* str2)
{
    while (strlen(str1) && strlen(str2))
    {
        if (*str1 != *str2)
        {
            return false;
        }
        str1++;
        str2++;
    }
    return true;
}

bool	parse(char * data, char	*username)
{
    int     i, n, j, k;
    char *name;
    
    n = get_id();
    i = 0;
    j = 0;
    while (data[i])
    {
        if (i == 0 && data[i] == '\n' || data[i] == ' ' || data[i] == '\t')
        {
            return false;
        }
        i += 2;
        j = i;
        while (data[i] && data[i] != '\n')
        {
            if (data[i] == ' ')
            {
                name = get_name(data, j, i);      
            }
            i++;           
        }
		if (compare(username, name))
		{
			return true;
		}
        i++;
    }
   
    return false;
}

bool    check_reg(char *username)
{
    FILE    *ptr;
    char    *buffer;
    int     i;
	char	**tmp;

    buffer = calloc(1024,1);
    ptr = open(USR,O_RDONLY);
    i = 1;
    while (i)
    {
        i = read(ptr,buffer,1024);
    }
    
	close(ptr);
    if (!parse(buffer, username)){
		free(buffer);
		return true;
	}
    
	free(buffer);
    return false;
}

void strrev(char* str)
{
    if (!str) {
        return;
    }
    int i = 0;
    int j = strlen(str) - 1;

    while (i < j) {
        char c = str[i];
        str[i] = str[j];
        str[j] = c;
        i++;
        j--;
    }
}
char *itoa(int nu)
{
    if (nu == 0) {
        return "0";
    }
    char    *number;
    int     n = nu;
    int     i = 0;
    number = calloc(1,12);
    while (n)
    {
       number[i++] = (n%10) + 48;
        n/=10;
    }
    strrev(number);
    return number;
}
void removeSpaces(char *str) {
    int count = 0; 
    for (int i = 0; str[i]; i++) {
        if (str[i] != ' ') {
            str[count++] = str[i];
        }
    }
    str[count] = '\0';
}


void	registerClient(struct User *u)
{
	FILE	*ptr;
    int      i, j;
   
    u->id = get_id();
    clearstr(2,u->name, u->password);
    write(1, "Please write your Username :\n", 30);
    read(0,u->name,50);
    removeSpaces(u->name);
    if (!check_reg(u->name))
    {
        write(1,"user already exists !\n",22);
        return;
    }
    write(1, "Please write your Password :\n", 30);
    read(0, u->password, 50);
    i = strlen(u->name);
    j = strlen(u->password);
    char *id = itoa(u->id);
    ptr = open(USR,O_WRONLY | O_APPEND);
    write(ptr, id, strlen(id));
    write(ptr, " ", 1);
   	write(ptr, u->name, i-1);
    write(ptr, " ", 1);
    write(ptr, u->password, j);
	close(ptr);
}

int getAccountFromFile(FILE *ptr, char name[50], struct Record *r)
{
    return fscanf(ptr, "%d %d %s %d %d/%d/%d %s %d %lf %s",
                  &r->id,
		  &r->userId,
		  name,
                  &r->accountNbr,
                  &r->deposit.month,
                  &r->deposit.day,
                  &r->deposit.year,
                  r->country,
                  &r->phone,
                  &r->amount,
                  r->accountType) != EOF;
}

void saveAccountToFile(FILE *ptr, struct User u, struct Record r)
{
    fprintf(ptr, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
            &r.id,
	        &u.id,
            r.accountNbr,
            r.deposit.month,
            r.deposit.day,
            r.deposit.year,
            r.country,
            r.phone,
            r.amount,
            r.accountType);
}

void stayOrReturn(int notGood, void f(struct User u), struct User u)
{
    int option;
    if (notGood == 0)
    {
        system("clear");
        printf("\n✖ Record not found!!\n");
    invalid:
        printf("\nEnter 0 to try again, 1 to return to main menu and 2 to exit:");
        scanf("%d", &option);
        if (option == 0)
            f(u);
        else if (option == 1)
            mainMenu(u);
        else if (option == 2)
            exit(0);
        else
        {
            printf("Insert a valid operation!\n");
            goto invalid;
        }
    }
    else
    {
        printf("\nEnter 1 to go to the main menu and 0 to exit:");
        scanf("%d", &option);
    }
    if (option == 1)
    {
        system("clear");
        mainMenu(u);
    }
    else
    {
        system("clear");
        exit(1);
    }
}

void success(struct User u)
{
    int option;
    printf("\n✔ Success!\n\n");
invalid:
    printf("Enter 1 to go to the main menu and 0 to exit!\n");
    scanf("%d", &option);
    system("clear");
    if (option == 1)
    {
        mainMenu(u);
    }
    else if (option == 0)
    {
        exit(1);
    }
    else
    {
        printf("Insert a valid operation!\n");
        goto invalid;
    }
}

void createNewAcc(struct User u)
{
    struct Record r;
    struct Record cr;
    char userName[50];
    FILE *pf = fopen(RECORDS, "a+");

noAccount:
    system("clear");
    printf("\t\t\t===== New record =====\n");

    printf("\nEnter today's date(mm/dd/yyyy):");
    scanf("%d/%d/%d", &r.deposit.month, &r.deposit.day, &r.deposit.year);
    printf("\nEnter the account number:");
    scanf("%d", &r.accountNbr);

    while (getAccountFromFile(pf, userName, &cr))
    {
        if (strcmp(userName, u.name) == 0 && cr.accountNbr == r.accountNbr)
        {
            printf("✖ This Account already exists for this user\n\n");
            goto noAccount;
        }
    }
    printf("\nEnter the country:");
    scanf("%s", r.country);
    printf("\nEnter the phone number:");
    scanf("%d", &r.phone);
    printf("\nEnter amount to deposit: $");
    scanf("%lf", &r.amount);
    printf("\nChoose the type of account:\n\t-> saving\n\t-> current\n\t-> fixed01(for 1 year)\n\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n\n\tEnter your choice:");
    scanf("%s", r.accountType);

    saveAccountToFile(pf, u, r);

    fclose(pf);
    success(u);
}

void checkAllAccounts(struct User u)
{
    char userName[100];
    struct Record r;

    FILE *pf = fopen(RECORDS, "r");

    system("clear");
    printf("\t\t====== All accounts from user, %s =====\n\n", u.name);
    while (getAccountFromFile(pf, userName, &r))
    {
        if (strcmp(userName, u.name) == 0)
        {
            printf("_____________________\n");
            printf("\nAccount number:%d\nDeposit Date:%d/%d/%d \ncountry:%s \nPhone number:%d \nAmount deposited: $%.2f \nType Of Account:%s\n",
                   r.accountNbr,
                   r.deposit.day,
                   r.deposit.month,
                   r.deposit.year,
                   r.country,
                   r.phone,
                   r.amount,
                   r.accountType);
        }
    }
    fclose(pf);
    success(u);
}
