#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX 50

/* =========================================================
   PORTALX - MINI COLLEGE BROWSER (Command Line Version)
   Pages : Home, Attendance, Timetable, Results

   CO1 : Back / Forward Navigation, Recently Closed Tabs - STACK
   CO2 : Tabs, Bookmarks & Browsing History           - LINKED LIST
   ========================================================= */

/* ---------------- STACK ---------------- */

typedef struct
{
    char pages[MAX][100];
    int top;
} Stack;

void initializeStack(Stack *s) { s->top = -1; }
int isEmpty(Stack *s) { return s->top == -1; }
int isFull(Stack *s) { return s->top == MAX - 1; }

void push(Stack *s, char page[])
{
    if (isFull(s))
    {
        printf("Stack is full!\n");
        return;
    }
    s->top++;
    strcpy(s->pages[s->top], page);
}

char *pop(Stack *s)
{
    if (isEmpty(s))
        return NULL;
    return s->pages[s->top--];
}

void displayStack(Stack *s)
{
    int i;
    if (isEmpty(s))
    {
        printf("  (empty)\n");
        return;
    }
    for (i = s->top; i >= 0; i--)
        printf("  %s%s\n", s->pages[i], i == s->top ? "   <- top" : "");
}

/* ---------------- TABS (Doubly Linked List) ---------------- */

typedef struct Tab
{
    int tabNumber;
    char page[100];
    struct Tab *prev;
    struct Tab *next;
} Tab;

Tab *createTab(int number, char page[])
{
    Tab *newTab = (Tab *)malloc(sizeof(Tab));
    if (newTab == NULL)
    {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    newTab->tabNumber = number;
    strcpy(newTab->page, page);
    newTab->prev = NULL;
    newTab->next = NULL;
    return newTab;
}

void addTab(Tab **head, int *tabCount, char page[])
{
    Tab *newTab = createTab(*tabCount + 1, page);
    if (newTab == NULL)
        return;

    if (*head == NULL)
        *head = newTab;
    else
    {
        Tab *temp = *head;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = newTab;
        newTab->prev = temp;
    }

    (*tabCount)++;
    printf("Tab %d opened: %s\n", newTab->tabNumber, page);
}

Tab *findTab(Tab *head, int tabNumber)
{
    while (head != NULL && head->tabNumber != tabNumber)
        head = head->next;
    return head;
}

void displayTabs(Tab *head)
{
    if (head == NULL)
    {
        printf("No tabs are currently open.\n");
        return;
    }
    printf("--- Open Tabs ---\n");
    while (head != NULL)
    {
        printf("  Tab %d : %s\n", head->tabNumber, head->page);
        head = head->next;
    }
}

void closeTab(Tab **head, int tabNumber, Stack *closedTabs)
{
    Tab *temp = findTab(*head, tabNumber);

    if (temp == NULL)
    {
        printf("Tab not found.\n");
        return;
    }

    if (temp->prev != NULL)
        temp->prev->next = temp->next;
    else
        *head = temp->next;

    if (temp->next != NULL)
        temp->next->prev = temp->prev;

    push(closedTabs, temp->page);   /* remember it for "reopen" */
    printf("Tab %d closed.\n", tabNumber);
    free(temp);
}

/* ---------------- BOOKMARKS (Singly Linked List) ---------------- */

typedef struct Bookmark
{
    char page[100];
    struct Bookmark *next;
} Bookmark;

int bookmarkExists(Bookmark *head, char page[])
{
    while (head != NULL)
    {
        if (strcmp(head->page, page) == 0)
            return 1;
        head = head->next;
    }
    return 0;
}

void addBookmark(Bookmark **head, char page[])
{
    Bookmark *newBookmark;

    if (bookmarkExists(*head, page))
    {
        printf("Already bookmarked: %s\n", page);
        return;
    }

    newBookmark = (Bookmark *)malloc(sizeof(Bookmark));
    if (newBookmark == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }
    strcpy(newBookmark->page, page);
    newBookmark->next = *head;
    *head = newBookmark;
    printf("Bookmark added: %s\n", page);
}

void displayBookmarks(Bookmark *head)
{
    int count = 1;
    if (head == NULL)
    {
        printf("No bookmarks saved.\n");
        return;
    }
    printf("--- Bookmarks ---\n");
    while (head != NULL)
    {
        printf("  %d. %s\n", count++, head->page);
        head = head->next;
    }
}

void searchBookmark(Bookmark *head, char page[])
{
    if (bookmarkExists(head, page))
        printf("Bookmark found: %s\n", page);
    else
        printf("Bookmark not found.\n");
}

void deleteBookmark(Bookmark **head, char page[])
{
    Bookmark *temp = *head;
    Bookmark *previous = NULL;

    while (temp != NULL)
    {
        if (strcmp(temp->page, page) == 0)
        {
            if (previous == NULL)
                *head = temp->next;
            else
                previous->next = temp->next;
            free(temp);
            printf("Bookmark deleted.\n");
            return;
        }
        previous = temp;
        temp = temp->next;
    }
    printf("Bookmark not found.\n");
}

/* ---------------- HISTORY (Singly Linked List) ---------------- */

typedef struct History
{
    int visitNumber;
    char page[100];
    struct History *next;
} History;

void addHistory(History **head, int *visitCount, char page[])
{
    History *newHistory = (History *)malloc(sizeof(History));
    if (newHistory == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }
    newHistory->visitNumber = ++(*visitCount);
    strcpy(newHistory->page, page);
    newHistory->next = NULL;

    if (*head == NULL)
        *head = newHistory;
    else
    {
        History *temp = *head;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = newHistory;
    }
}

void displayHistory(History *head)
{
    if (head == NULL)
    {
        printf("Browsing history is empty.\n");
        return;
    }
    printf("--- Browsing History ---\n");
    while (head != NULL)
    {
        printf("  %d. %s\n", head->visitNumber, head->page);
        head = head->next;
    }
}

void clearHistory(History **head, int *visitCount)
{
    History *temp;
    while (*head != NULL)
    {
        temp = *head;
        *head = (*head)->next;
        free(temp);
    }
    *visitCount = 0;
    printf("History cleared.\n");
}

/* ---------------- PAGES ---------------- */

static const char *PAGE_NAMES[] = { "Home", "Attendance", "Timetable", "Results" };
#define PAGE_COUNT 4

void displayPages()
{
    int i;
    printf("--- PortalX Pages ---\n");
    for (i = 0; i < PAGE_COUNT; i++)
        printf("  %d. %s\n", i + 1, PAGE_NAMES[i]);
}

/* What each page "shows" - plain text, no UI */
void showPageContent(char page[])
{
    printf("\n=========== %s ===========\n", page);

    if (strcmp(page, "Home") == 0)
    {
        printf("Welcome to PortalX - your college portal.\n");
        printf("Quick links : Attendance | Timetable | Results\n");
        printf("Notice      : Mid-semester exams begin next Monday.\n");
    }
    else if (strcmp(page, "Attendance") == 0)
    {
        printf("%-22s %8s %8s\n", "Subject", "Held", "Present");
        printf("%-22s %8d %8d\n", "Data Structures",   40, 36);
        printf("%-22s %8d %8d\n", "Discrete Maths",    38, 31);
        printf("%-22s %8d %8d\n", "Computer Org.",     36, 34);
        printf("%-22s %8d %8d\n", "Python Programming", 42, 35);
        printf("Overall attendance: 85.9%%\n");
    }
    else if (strcmp(page, "Timetable") == 0)
    {
        printf("Mon : Data Structures | Discrete Maths | Lab\n");
        printf("Tue : Computer Org.   | Python         | Lab\n");
        printf("Wed : Discrete Maths  | Data Structures | Seminar\n");
        printf("Thu : Python          | Computer Org.  | Lab\n");
        printf("Fri : Data Structures | Mentoring      | Sports\n");
    }
    else if (strcmp(page, "Results") == 0)
    {
        printf("%-22s %8s %8s\n", "Subject", "Marks", "Grade");
        printf("%-22s %8d %8s\n", "Data Structures",   82, "A");
        printf("%-22s %8d %8s\n", "Discrete Maths",    74, "B+");
        printf("%-22s %8d %8s\n", "Computer Org.",     79, "A-");
        printf("%-22s %8d %8s\n", "Python Programming", 88, "A+");
        printf("SGPA: 8.7\n");
    }
    printf("==================================\n");
}

void toLowerStr(char *dst, const char *src)
{
    int i;
    for (i = 0; src[i]; i++)
        dst[i] = (char)tolower((unsigned char)src[i]);
    dst[i] = '\0';
}

/* Matches typed name (case-insensitive, prefix allowed) or number 1-4 */
int findPage(char input[], char page[])
{
    char a[200], b[100];
    int i, num;

    if (strlen(input) == 0)
        return 0;

    if (sscanf(input, "%d", &num) == 1 && num >= 1 && num <= PAGE_COUNT)
    {
        strcpy(page, PAGE_NAMES[num - 1]);
        return 1;
    }

    toLowerStr(a, input);
    for (i = 0; i < PAGE_COUNT; i++)
    {
        toLowerStr(b, PAGE_NAMES[i]);
        if (strncmp(b, a, strlen(a)) == 0)
        {
            strcpy(page, PAGE_NAMES[i]);
            return 1;
        }
    }
    return 0;
}

/* ---------------- NAVIGATION ---------------- */

void visitPage(char currentPage[], Stack *backStack, Stack *forwardStack,
               History **history, int *visitCount, char newPage[])
{
    if (strlen(currentPage) > 0)
        push(backStack, currentPage);

    strcpy(currentPage, newPage);
    initializeStack(forwardStack);   /* a new path clears Forward */
    addHistory(history, visitCount, newPage);

    showPageContent(currentPage);
}

void goBack(char currentPage[], Stack *backStack, Stack *forwardStack)
{
    char previousPage[100];

    if (isEmpty(backStack))
    {
        printf("Cannot go Back. Back stack is empty.\n");
        return;
    }
    push(forwardStack, currentPage);
    strcpy(previousPage, pop(backStack));
    strcpy(currentPage, previousPage);
    showPageContent(currentPage);
}

void goForward(char currentPage[], Stack *backStack, Stack *forwardStack)
{
    char nextPage[100];

    if (isEmpty(forwardStack))
    {
        printf("Cannot go Forward. Forward stack is empty.\n");
        return;
    }
    push(backStack, currentPage);
    strcpy(nextPage, pop(forwardStack));
    strcpy(currentPage, nextPage);
    showPageContent(currentPage);
}

/* ---------------- FREE MEMORY ---------------- */

void freeTabs(Tab *head)
{
    Tab *temp;
    while (head != NULL)
    {
        temp = head;
        head = head->next;
        free(temp);
    }
}

void freeBookmarks(Bookmark *head)
{
    Bookmark *temp;
    while (head != NULL)
    {
        temp = head;
        head = head->next;
        free(temp);
    }
}

void freeHistory(History *head)
{
    History *temp;
    while (head != NULL)
    {
        temp = head;
        head = head->next;
        free(temp);
    }
}

/* ---------------- HELP ---------------- */

void showHelp()
{
    printf("\nNavigation (Stack)\n");
    printf("  visit <page>    go to a page         back / forward   move in history\n");
    printf("  backstack       show back stack      fwdstack         show forward stack\n");
    printf("  current         show current page    pages            list pages\n");
    printf("\nTabs (Doubly Linked List)\n");
    printf("  open <page>     open a new tab       close <n>        close tab n\n");
    printf("  switch <n>      visit the page in tab n\n");
    printf("  reopen          reopen last closed tab (Stack)        tabs   list tabs\n");
    printf("\nBookmarks & History (Linked List)\n");
    printf("  bookmark        bookmark current page    unbookmark <page>   remove\n");
    printf("  find <page>     search bookmarks         bookmarks           list\n");
    printf("  history         show history             clearhistory        wipe it\n");
    printf("\n  help   exit\n");
    printf("<page> = name, prefix (att, time, res) or number (see 'pages').\n");
}

/* ---------------- MAIN ---------------- */

int main(int argc, char *argv[])
{
    Stack backStack, forwardStack, closedTabs;
    Tab *tabHead = NULL, *t;
    Bookmark *bookmarkHead = NULL;
    History *historyHead = NULL;

    char currentPage[100] = "Home";
    char page[100], line[200], cmd[20];
    char *arg;
    int tabCount = 0, visitCount = 0, i, n;

    initializeStack(&backStack);
    initializeStack(&forwardStack);
    initializeStack(&closedTabs);

    addHistory(&historyHead, &visitCount, currentPage);

    printf("========================================\n");
    printf("      PORTALX - MINI COLLEGE BROWSER\n");
    printf("========================================\n");
    printf("Type 'help' to see all commands.\n");

    /* Command-line arguments: ./portalx attendance results */
    for (i = 1; i < argc; i++)
    {
        if (findPage(argv[i], page))
            visitPage(currentPage, &backStack, &forwardStack,
                      &historyHead, &visitCount, page);
        else
            printf("Unknown page argument: %s\n", argv[i]);
    }

    if (argc == 1)
        showPageContent(currentPage);

    while (1)
    {
        printf("\nportalx [%s]> ", currentPage);
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL)
            break;

        line[strcspn(line, "\n")] = '\0';

        arg = line;
        while (isspace((unsigned char)*arg)) arg++;
        if (*arg == '\0') continue;

        if (sscanf(arg, "%19s", cmd) != 1) continue;
        arg += strlen(cmd);
        while (isspace((unsigned char)*arg)) arg++;
        toLowerStr(cmd, cmd);

        if (strcmp(cmd, "visit") == 0)
        {
            if (findPage(arg, page))
                visitPage(currentPage, &backStack, &forwardStack,
                          &historyHead, &visitCount, page);
            else
                printf("Unknown page. Type 'pages' to see the list.\n");
        }
        else if (strcmp(cmd, "back") == 0)
            goBack(currentPage, &backStack, &forwardStack);
        else if (strcmp(cmd, "forward") == 0)
            goForward(currentPage, &backStack, &forwardStack);
        else if (strcmp(cmd, "backstack") == 0)
        {
            printf("--- Back Stack ---\n");
            displayStack(&backStack);
        }
        else if (strcmp(cmd, "fwdstack") == 0)
        {
            printf("--- Forward Stack ---\n");
            displayStack(&forwardStack);
        }
        else if (strcmp(cmd, "open") == 0)
        {
            if (findPage(arg, page))
                addTab(&tabHead, &tabCount, page);
            else
                printf("Unknown page. Type 'pages' to see the list.\n");
        }
        else if (strcmp(cmd, "close") == 0)
        {
            if (sscanf(arg, "%d", &n) == 1)
                closeTab(&tabHead, n, &closedTabs);
            else
                printf("Usage: close <tab number>\n");
        }
        else if (strcmp(cmd, "switch") == 0)
        {
            if (sscanf(arg, "%d", &n) == 1 && (t = findTab(tabHead, n)) != NULL)
            {
                strcpy(page, t->page);
                visitPage(currentPage, &backStack, &forwardStack,
                          &historyHead, &visitCount, page);
            }
            else
                printf("Usage: switch <tab number> (see 'tabs')\n");
        }
        else if (strcmp(cmd, "reopen") == 0)
        {
            if (isEmpty(&closedTabs))
                printf("No closed tabs to reopen.\n");
            else
                addTab(&tabHead, &tabCount, pop(&closedTabs));
        }
        else if (strcmp(cmd, "tabs") == 0)
            displayTabs(tabHead);
        else if (strcmp(cmd, "bookmark") == 0)
            addBookmark(&bookmarkHead, currentPage);
        else if (strcmp(cmd, "unbookmark") == 0)
        {
            if (!findPage(arg, page)) { strncpy(page, arg, 99); page[99] = '\0'; }
            deleteBookmark(&bookmarkHead, page);
        }
        else if (strcmp(cmd, "find") == 0)
        {
            if (!findPage(arg, page)) { strncpy(page, arg, 99); page[99] = '\0'; }
            searchBookmark(bookmarkHead, page);
        }
        else if (strcmp(cmd, "bookmarks") == 0)
            displayBookmarks(bookmarkHead);
        else if (strcmp(cmd, "history") == 0)
            displayHistory(historyHead);
        else if (strcmp(cmd, "clearhistory") == 0)
            clearHistory(&historyHead, &visitCount);
        else if (strcmp(cmd, "current") == 0)
            showPageContent(currentPage);
        else if (strcmp(cmd, "pages") == 0)
            displayPages();
        else if (strcmp(cmd, "help") == 0)
            showHelp();
        else if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0)
        {
            printf("Thank you for using PortalX!\n");
            break;
        }
        else
            printf("Unknown command. Type 'help'.\n");
    }

    freeTabs(tabHead);
    freeBookmarks(bookmarkHead);
    freeHistory(historyHead);

    return 0;
}
