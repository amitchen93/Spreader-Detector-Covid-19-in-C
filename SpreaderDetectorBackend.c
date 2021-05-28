//
// Created by amitc on 02/08/2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SpreaderDetectorParams.h"

#define INPUT_ERROR "Error in input files.\n"
#define OUTPUT_ERROR "Error in output file.\n"
#define BAD_FORMAT "Usage: ./SpreaderDetectorBackend <Path to People.in> <Path to Meetings.in>\n"

#define READING_MODE "r"
#define WRITING_MODE "w"

#define MAX_LINE_LENGTH 1025
#define ID_LENGTH 10
#define BASE 10
#define ARGS_NUM 3
#define TRUE 1
#define FALSE 0

/** DECLERATIONS */

typedef struct Person Person;
typedef struct NodeTree NodeTree;
typedef struct Array Array;

NodeTree *newNodeTree(Person *person);

void insertNodeToTree(NodeTree **TreeToInsertTo, NodeTree *newNode);

Person *getPersonFromTreeWithID(NodeTree **rootOfPerson, int ID);
void checkArgs(int argc, char **argv);

void writePersonToOutputFile(FILE *output, const Person *person);
void recursiveTravelOnTreeOfProb(FILE *output, const NodeTree *NodeOfProb);
void freeIdTree(NodeTree *rootOfDeleteNode);

void freeProbTree(NodeTree *rootOfDeleteNode);

void careOfOutputFile(NodeTree *headOfProbTree);

void standardLibraryErrorPrint();

int convertStringToInt(const char *string);

int getPartsFromPeople();

float crna(float probOfPrevPerson, float dist, float time);

void insertToProbTree(Person *personFromTree, NodeTree *probTree);

void inOrderIteratorOnTree(NodeTree *rootOfTree, NodeTree *probTree);

void getPartsOfMeet();

NodeTree *createArrayOfProb();

/** PLEASE LOOK AT README (WHY I USE GLOBAL) */
// create empty tree, in while loop we put person in it
NodeTree *root = NULL;
FILE *peopleFile;
FILE *meetingsFile;


/** structs: */

/**
 * data structure for Person node
 */
struct Person
{
    char *name;
    int ID;
    char *age;
    float probability;

};

/**
 * @brief Node for Tree
 */
struct NodeTree
{
    Person *_person;
    struct NodeTree *left;
    struct NodeTree *right;
};

/**
 * @brief create new node for Tree
 * @param - person no const cause we get access to
 */
NodeTree *newNodeTree(Person *person)
{
    NodeTree *newNode = (NodeTree *) malloc(sizeof(NodeTree));
    if (newNode == NULL)
    {
        standardLibraryErrorPrint();
    }
    newNode->_person = person;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

/**
 * @brief  insert new node for Tree -  pointer to pointer cause we have
 * pointer in function, in another
 * words - we want to change the item from *outside the function (TA5 - 01:57:00)
 * @param TreeToInsertTo
 * @param newNode no const cause we get access to
 */
void insertNodeToTree(NodeTree **TreeToInsertTo, NodeTree *newNode)
{
    if (*TreeToInsertTo == NULL) // build a tree (new TreeToInsertTo)
    {
        *TreeToInsertTo = newNode; // TreeToInsertTo is the newNode
    }
    else
    {
        if (newNode->_person->ID > (*TreeToInsertTo)->_person->ID)
        {
            insertNodeToTree(&(*TreeToInsertTo)->right, newNode);
        }
        else if (newNode->_person->ID < (*TreeToInsertTo)->_person->ID)
        {
            insertNodeToTree(&(*TreeToInsertTo)->left, newNode);
        }
    }
}

/**
 * @brief get the Person From Tree With his ID
 * @param rootOfPerson
 * @param ID
 * @return Person
 */
Person *getPersonFromTreeWithID(NodeTree **rootOfPerson, int ID)
{
    if (*rootOfPerson == NULL)
    {
        return 0; // empty tree
    }
    const NodeTree *curNode = *rootOfPerson;

    if (ID == curNode->_person->ID)
    {
        return curNode->_person;
    }
    else if (ID > curNode->_person->ID) // we go right cause we bigger than node
    {
        return getPersonFromTreeWithID(&(*rootOfPerson)->right, ID);
    }
    else
    {
        return getPersonFromTreeWithID(&(*rootOfPerson)->left,
                                       ID); // we go left cause we bigger than node
    }

}

/**
 * @brief check the args are 3 and valid
 * @param argc
 * @param argv
 */
void checkArgs(int argc, char **argv)
{
    //first -  lets check the args
    if (argc != ARGS_NUM)
    {
        fprintf(stderr, BAD_FORMAT);
        exit(EXIT_FAILURE);
        // nothing to free
    }

    //second -  check if the file exist

    peopleFile = fopen(argv[1], READING_MODE);
    meetingsFile = fopen(argv[2], READING_MODE);

    // check if one of the files don't exist - error with open file
    if (peopleFile == NULL || meetingsFile == NULL)
    {
        fprintf(stderr, INPUT_ERROR);
        fclose(peopleFile);
        fclose(meetingsFile);
        exit(EXIT_FAILURE);
        // nothing to free
    }

}

/**
 * @brief we know the data of person. so we write his data to output file
 * and do this to all person with "recursiveTravelOnTreeOfProb"
 * if prob >= MEDICAL_SUPERVISION_THRESHOLD -> MEDICAL_SUPERVISION_THRESHOLD_MSG
 * if prob >= REGULAR_QUARANTINE_THRESHOLD -> REGULAR_QUARANTINE_MSG
 * else -> CLEAN_MSG
 * @param output file
 * @param person we write to output file
 */
void writePersonToOutputFile(FILE *output, const Person *person)
{
    if (person->probability >= MEDICAL_SUPERVISION_THRESHOLD)
    {
        fprintf(output, MEDICAL_SUPERVISION_THRESHOLD_MSG,
                person->name, (unsigned long) person->ID);
        return;
    }
    if (person->probability >= REGULAR_QUARANTINE_THRESHOLD)
    {
        fprintf(output, REGULAR_QUARANTINE_MSG, person->name, (unsigned long) person->ID);
        return;
    }
    fprintf(output, CLEAN_MSG, person->name, (unsigned long) person->ID);
}

/**
 * @brief travel on tree inorder (from biggest - right, to smallest - left)
 * and use "writePersonToOutputFile"
 * @param output file
 * @param NodeOfProb from tree of prob we sorted
 */
void recursiveTravelOnTreeOfProb(FILE *output, const NodeTree *NodeOfProb)
{
    if (NodeOfProb == NULL)
    {
        return;
    }
    recursiveTravelOnTreeOfProb(output, NodeOfProb->right);
    writePersonToOutputFile(output, NodeOfProb->_person);
    recursiveTravelOnTreeOfProb(output, NodeOfProb->left);
}

/**
 * @brief free memory of ID Tree
 * @param rootOfDeleteNode
 */
void freeIdTree(NodeTree *rootOfDeleteNode)
{
    if (rootOfDeleteNode == NULL) // empty tree
    {
        return;
    }
    freeIdTree(rootOfDeleteNode->right);
    freeIdTree(rootOfDeleteNode->left);

    // free to root
    free(rootOfDeleteNode->_person->name);
    free(rootOfDeleteNode->_person->age);
    free(rootOfDeleteNode->_person);
    free(rootOfDeleteNode);
}

/**
 * @brief free memory of Prob Tree
 * @param rootOfDeleteNode
 */
void freeProbTree(NodeTree *rootOfDeleteNode)
{
    if (rootOfDeleteNode == NULL) // empty tree
    {
        return;
    }
    freeProbTree(rootOfDeleteNode->right);
    freeProbTree(rootOfDeleteNode->left);

    // free to root
    free(rootOfDeleteNode);
}

/**
 * @brief create output file with info about hospital / home / healthy, also free data and exit
 */
void careOfOutputFile(NodeTree *headOfProbTree)
{
    // create output file
    FILE *outputFile = fopen(OUTPUT_FILE, WRITING_MODE);
    if (outputFile == NULL)
    {
        fprintf(stderr, OUTPUT_ERROR);
        // free tree
        freeIdTree(root);
        freeProbTree(headOfProbTree);
        exit(EXIT_FAILURE);

    }
    if (headOfProbTree->_person != NULL)
    {
        recursiveTravelOnTreeOfProb(outputFile, headOfProbTree);
    }
    fclose(outputFile);

    // free memory
    freeIdTree(root);
    root = NULL;
    freeProbTree(headOfProbTree);
    headOfProbTree = NULL;
}


/**
 * @brief prinf to stderr a msg about standard Library error and, free data and exit
 */
void standardLibraryErrorPrint()
{
    fprintf(stderr, STANDARD_LIB_ERR_MSG);
    exit(EXIT_FAILURE);
}

/**
 * @brief a helper function that convert a given string to integer using strtol.
 * the program will not send here a string that is not a number,
 * @param string - string to convert.
 * @param line - the line that the string came from
 * @return - the converted integer.
 */
int convertStringToInt(const char *string)
{
    // in case we get a number such as "021"
    if (strlen(string) > 1 && string[0] == '0')
    {
        return -1;
    }

    for (int i = 0; i < (int) strlen(string); ++i)
    {
        if (string[i] > '9' || string[i] < '0')
        {
            return -1;
        }
    }
    // no const cause we get access to
    int num;
    char *ptr;
    num = (int) strtol(string, &ptr, BASE); // BASE = 10
    return num;
}


/**
 * @brief runs on the file "people" and gets parts into a tree.
 */
int getPartsFromPeople()
{
    int lineOfPeopleFile = 0; // no const cause we get access to
    char input_line[MAX_LINE_LENGTH]; // buffer of 1025 chars. no const cause we get access to


    // read a line from "peopleFile"
    while (fgets(input_line, MAX_LINE_LENGTH, peopleFile) != NULL)
    {

        // alloc Person to put information in
        Person *currPerson = (Person *) malloc(sizeof(Person));
        if (currPerson == NULL)
        {
            fclose(peopleFile);
            standardLibraryErrorPrint();
        }

        // create buffers
        char *nameFromInput = (char *) malloc(MAX_LINE_LENGTH); // 1025
        char *IDFromInput = (char *) malloc(ID_LENGTH); //ID=9 char*
        char *ageFromInput = (char *) malloc(MAX_LINE_LENGTH); //age= [0-120] // 1025
        if (nameFromInput == NULL || IDFromInput == NULL || ageFromInput == NULL)
        {
            fclose(peopleFile);
            free(nameFromInput);
            nameFromInput = NULL;

            free(IDFromInput);
            IDFromInput = NULL;

            free(ageFromInput);
            ageFromInput = NULL;
            standardLibraryErrorPrint();
        }


        // scan data
        sscanf(input_line, "%s %s %s\n", nameFromInput, IDFromInput, ageFromInput);


        // put the input in Person struct (each line for Person)
        currPerson->name = nameFromInput;
        currPerson->ID = convertStringToInt(IDFromInput);
        currPerson->age = ageFromInput;
        currPerson->probability = 0; // change prob to 0 (in second function I update the prob)



        // put the currperson (node) we read from line into a node of tree
        insertNodeToTree(&root, newNodeTree(currPerson));


        lineOfPeopleFile++; // upgrade int line (go to next line in input)
        free(IDFromInput);
        IDFromInput = NULL;
    }

    fclose(peopleFile); // we don't need the input file anymore
    return lineOfPeopleFile;
}

/**
 * @brief calc prob to be sick by the given formula
 * @param prob Of Prev Person in the same line in meetings file
 * @param other
 * @param dist
 * @param time
 * @return
 */
float crna(float probOfPrevPerson, float dist, float time)
{
    const float mone = MIN_DISTANCE * time;
    const float mechane = MAX_TIME * dist;
    return (mone / mechane) * probOfPrevPerson;
}

/**
 * @brief insert a person to probTree
 * @param personFromTree - the person from the original tree. no const cause we get access to
 * @param probTree - tree of probability
 */
void insertToProbTree(Person *personFromTree, NodeTree *probTree)
{
    if (probTree->_person == NULL) // if the NEW tree is empty
    {
        probTree->_person = personFromTree;
        return;
    }
    // if
    while (1)// true
    {
        // if my prob smaller than the prob of my parent
        if (personFromTree->probability < probTree->_person->probability)
        {
            if (probTree->left == NULL) // if I have no left child
            {
                probTree->left = newNodeTree(personFromTree);
                break;
            }
            else // if I have left child
            {
                probTree = probTree->left;  // continue travel in tree to the left
                continue;
            }
        }
        else // if my prob bigger than the prob of my parent
        {
            if (probTree->right == NULL) // if I have no right child
            {
                probTree->right = newNodeTree(personFromTree);
                break;
            }
            else // if I have right child
            {
                probTree = probTree->right;  // continue travel in tree to the right
                continue;
            }
        }
    }
}

/**
 * @brief recursive call of nodes of tree - kind of iterator on the original tree
 * @param rootOfTree
 * @param probTree
 */
void inOrderIteratorOnTree(NodeTree *rootOfTree, NodeTree *probTree)
{
    if (rootOfTree == NULL)
    {
        return; // empty tree
    }

    inOrderIteratorOnTree(rootOfTree->left, probTree); // recursive call
    inOrderIteratorOnTree(rootOfTree->right, probTree); // recursive call
    insertToProbTree(rootOfTree->_person, probTree);
}


/**
 * @brief pharse the file "meetings and update the prob of each person
 */
void getPartsOfMeet()
{
    char input_line[MAX_LINE_LENGTH]; // 1025 chars
    char *IDOfSickStr = (char *) malloc(MAX_LINE_LENGTH); // 1025
    if (IDOfSickStr == NULL)
    {
        fclose(meetingsFile);
        standardLibraryErrorPrint();
    }

    /** first line - root (sick) */

    // read first line of meetingsFile (ID of root)
    if (fgets(input_line, MAX_LINE_LENGTH, meetingsFile) == NULL)
    {
        fclose(meetingsFile);
        free(IDOfSickStr);
        IDOfSickStr = NULL;
        return;
    }
    // scan data in first line
    sscanf(input_line, "%s\n", IDOfSickStr);
    const int IDOfSick = convertStringToInt(IDOfSickStr);
    //free string buffer
    free(IDOfSickStr);
    IDOfSickStr = NULL;


    //return the person that appears in head file of meeting (the sick)
    Person *person = getPersonFromTreeWithID(&root, IDOfSick);
    person->probability = 1; //update his prob to 1.0(float) cause he is sick surly



    // read a line from "meeting" - START FROM LINE 2
    while (fgets(input_line, MAX_LINE_LENGTH, meetingsFile) != NULL)
    {
        // create buffers
        char *maybeSickIDFromInput = (char *) malloc(MAX_LINE_LENGTH); //1025
        char *otherPersonIDFromInput = (char *) malloc(ID_LENGTH); //ID len 9 (char *)
        char *distanceFromInput = (char *) malloc(MAX_LINE_LENGTH); // 1025
        char *timeFromInput = (char *) malloc(MAX_LINE_LENGTH); //1025

        if (maybeSickIDFromInput == NULL || otherPersonIDFromInput == NULL ||
            distanceFromInput == NULL || timeFromInput == NULL)
        {
            fclose(meetingsFile);
            free(maybeSickIDFromInput);
            maybeSickIDFromInput = NULL;

            free(otherPersonIDFromInput);
            otherPersonIDFromInput = NULL;

            free(distanceFromInput);
            distanceFromInput = NULL;

            free(timeFromInput);
            timeFromInput = NULL;

            standardLibraryErrorPrint();
        }

        // scan data
        sscanf(input_line, "%s %s %s %s\n", maybeSickIDFromInput, otherPersonIDFromInput,
               distanceFromInput, timeFromInput);


        // convert Id of wto people to int (to find them in tree)
        int maybeSickID = convertStringToInt(maybeSickIDFromInput);
        int otherPersonID = convertStringToInt(otherPersonIDFromInput);

        //free buffers
        free(maybeSickIDFromInput);
        free(otherPersonIDFromInput);

        // convert dis and time from strings to float number (to calc with crna)
        char *pendForDis;
        const float dis = strtof(distanceFromInput, &pendForDis);
        char *pendForTime;
        const float time = strtof(timeFromInput, &pendForTime);

        //free buffers
        free(distanceFromInput);
        distanceFromInput = NULL;

        free(timeFromInput);
        timeFromInput = NULL;

        /** we want to calculate prob of other man (we have the first man from last iteration) */
        //find 2 people in tree by their ID
        Person *personPrev = getPersonFromTreeWithID(&root, maybeSickID);
        Person *personCurr = getPersonFromTreeWithID(&root, otherPersonID);
        personCurr->probability = crna(personPrev->probability, dis, time);
        // we update the prob of curPerson
    }
    fclose(meetingsFile);
}

/**
 * @brief we create a tree that sorted by probability of each person
 * @return the tree after sorting
 */
NodeTree *createArrayOfProb()
{
    NodeTree *probTree = newNodeTree(NULL);

    inOrderIteratorOnTree(root, probTree);
    return probTree;
}


/**
 * @brief main fuction that manage the program
 * @param argc
 * @param argv
 * @return 0
 */
int main(int argc, char *argv[])
{
    checkArgs(argc, argv);
    getPartsFromPeople();
    getPartsOfMeet();
    NodeTree *sortProbTree = createArrayOfProb();  // get sorted tree by prob
    careOfOutputFile(sortProbTree);

    return 0;
}


