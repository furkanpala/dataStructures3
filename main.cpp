/* @Author
Student Name: Furkan Pala
Student ID: 150180109
Date: 18/12/2019 */

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// MESSAGE STRUCTURES
struct Message
{
    string content;
    int target_id;
    Message *next;
};

struct MessageQueue
{
    Message *front;
    Message *back;
    void create();
    void close();
    void enqueue(Message *);
    Message *dequeue();
    bool isEmpty();
};
// MESSAGE STRUCTURES END

// STACK STRUCTURE
struct BaseStationIDNode
{
    int id;
    BaseStationIDNode *next;
};
struct BaseStationIDStack
{
    BaseStationIDNode *head;
    void create();
    void close();
    void push(int id);
    int pop();
    bool isEmpty();
    void print();
    void makeEmpty();
};
// STACK STRUCTURE END

// NETWORK STRUCTURES
struct MobileHost
{
    MobileHost *right;
    int id;
    int parent_id;
};

struct BaseStation
{
    BaseStation *child;
    BaseStation *right;
    MobileHost *counterPart;
    int id;
    int parent_id;
    MobileHost *getMobileHost(MobileHost *, int id);
};

struct Network
{
    BaseStation *centralController;
    void create();
    void emptyNetwork(BaseStation *);
    void close();
    void insertBS(BaseStation *);
    void insertMH(MobileHost *);
    void DFS_traverse(BaseStation *);
    BaseStation *findBS(BaseStation *, int id);
    MobileHost *findMH(BaseStation *, int id);
    bool getPath(BaseStation *, BaseStation *, BaseStationIDStack *);
    void processMessages(MessageQueue *, BaseStationIDStack *);
};
// NETWORK STRUCTURES END

// NETWORK METHODS
void Network::create()
{
    centralController = new BaseStation;
    centralController->right = NULL;
    centralController->child = NULL;
    centralController->counterPart = NULL;
    centralController->id = 0;
    centralController->parent_id = 0;
}

MobileHost *BaseStation::getMobileHost(MobileHost *MH, int id)
{
    if (MH == NULL)
        return NULL;
    if (MH->id == id)
        return MH;
    return getMobileHost(MH->right, id);
}

void Network::insertBS(BaseStation *newBS)
{
    if (centralController->child == NULL)
        centralController->child = newBS;
    else
    {
        BaseStation *parentBS = findBS(centralController, newBS->parent_id);
        if (parentBS)
        {
            if (parentBS->child == NULL)
                parentBS->child = newBS;
            else
            {
                BaseStation *traverse = parentBS->child;
                while (traverse->right)
                    traverse = traverse->right;
                traverse->right = newBS;
            }
        }
    }
}

void Network::DFS_traverse(BaseStation *BS)
{
    cout << BS->id << " ";

    BaseStation *traverse = BS->child;
    while (traverse)
    {
        DFS_traverse(traverse);
        traverse = traverse->right;
    }
}

BaseStation *Network::findBS(BaseStation *BS, int id)
{
    if (BS->id == id)
        return BS;
    BaseStation *traverse = BS->child;
    BaseStation *found;
    while (traverse)
    {
        found = findBS(traverse, id);
        if (found)
            return found;
        traverse = traverse->right;
    }
    return NULL;
}

void Network::insertMH(MobileHost *newMH)
{
    BaseStation *parentBS = findBS(centralController, newMH->parent_id);
    if (parentBS)
    {
        if (parentBS->counterPart == NULL)
            parentBS->counterPart = newMH;
        else
        {
            MobileHost *traverse = parentBS->counterPart;
            while (traverse->right)
                traverse = traverse->right;
            traverse->right = newMH;
        }
    }
}

MobileHost *Network::findMH(BaseStation *BS, int id)
{
    cout << BS->id;
    MobileHost *MH = BS->getMobileHost(BS->counterPart, id);
    if (MH)
        return MH;
    cout << " ";

    BaseStation *traverse = BS->child;
    while (traverse)
    {
        MH = findMH(traverse, id);
        if (MH)
            return MH;
        traverse = traverse->right;
    }
    return NULL;
}

bool Network::getPath(BaseStation *root, BaseStation *BS, BaseStationIDStack *stack)
{
    bool found = false;
    if (root->id == BS->id)
    {
        stack->push(root->id);
        return true;
    }

    if (root->child)
    {
        if ((found = getPath(root->child, BS, stack)))
        {
            stack->push(root->id);
            return found;
        }
    }

    if (root->right)
    {
        if ((found = getPath(root->right, BS, stack)))
        {
            return found;
        }
    }
    stack->print();
    return found;
}

void Network::processMessages(MessageQueue *queue, BaseStationIDStack *stack)
{
    while (!(queue->isEmpty()))
    {
        Message *nextMessage;
        MobileHost *targetMH;
        nextMessage = queue->dequeue();
        cout << "Traversing:";
        targetMH = findMH(centralController, nextMessage->target_id);

        cout << endl;

        if (targetMH == NULL)
            cout << "Can not be reached the mobile host mh_" << nextMessage->target_id << " at the moment" << endl;
        else
        {
            stack->makeEmpty();
            BaseStation *parentBSofTargetMH = findBS(centralController, targetMH->parent_id);
            getPath(centralController, parentBSofTargetMH, stack);
            cout << "Message:" << nextMessage->content << " To:";
            stack->print();
            cout << "mh_" << nextMessage->target_id << endl;
        }
    }
}

void Network::emptyNetwork(BaseStation *BS)
{
    if (BS && BS->child == NULL)
    {
        BaseStation *p;
        while (BS)
        {
            p = BS;
            BS = BS->right;
            MobileHost *t;
            while (p->counterPart)
            {
                t = p->counterPart;
                p->counterPart = p->counterPart->right;
                delete t;
            }
            delete p;
        }

        return;
    }

    BaseStation *traverse = BS;
    BaseStation *traverse2 = traverse;
    while (traverse2)
    {
        traverse = traverse2;
        emptyNetwork(traverse2->child);
        traverse2->child = NULL;
        traverse2 = traverse;
        traverse2 = traverse2->right;
        delete traverse;
    }
}

void Network::close()
{
    emptyNetwork(centralController);
    centralController = NULL;
}
// NETWORK METHODS END

// MESSAGE METHODS
void MessageQueue::create()
{
    front = NULL;
    back = NULL;
}

bool MessageQueue::isEmpty()
{
    return front == NULL;
}

void MessageQueue::enqueue(Message *newMessage)
{
    if (isEmpty())
    {
        front = newMessage;
        back = front;
    }
    else
    {
        back->next = newMessage;
        back = newMessage;
    }
}

Message *MessageQueue::dequeue()
{
    Message *foremostMessage;
    foremostMessage = front;
    front = front->next;
    return foremostMessage;
}

void MessageQueue::close()
{
    Message *traverse;
    while (front)
    {
        traverse = front;
        front = front->next;
        delete traverse;
    }
    front = NULL;
    back = NULL;
}
// MESSAGE METHODS END

// STACK METHODS
void BaseStationIDStack::create()
{
    head = NULL;
}

bool BaseStationIDStack::isEmpty()
{
    return head == NULL;
}

void BaseStationIDStack::push(int id)
{
    BaseStationIDNode *BS_id = new BaseStationIDNode;
    BS_id->id = id;
    BS_id->next = head;
    head = BS_id;
}

int BaseStationIDStack::pop()
{
    BaseStationIDNode *foremostID;
    int temp;
    foremostID = head;
    head = head->next;
    temp = foremostID->id;
    delete foremostID;
    return temp;
}

void BaseStationIDStack::close()
{
    BaseStationIDNode *p;
    while (head)
    {
        p = head;
        head = head->next;
        delete p;
    }
    head = NULL;
}

void BaseStationIDStack::print()
{
    BaseStationIDNode *traverse = head;
    while (traverse)
    {
        cout << traverse->id << " ";
        traverse = traverse->next;
    }
}

void BaseStationIDStack::makeEmpty()
{
    while ((!isEmpty()))
        pop();
}
// STACK METHODS END

int main(int argc, char *argv[])
{
    Network network;
    network.create();

    MessageQueue queue;
    queue.create();

    BaseStationIDStack stack;
    stack.create();

    char *networkFileName, *messagesFileName;
    networkFileName = argv[1];
    messagesFileName = argv[2];

    ifstream inFile;

    // READ NETWORK FILE
    inFile.open(networkFileName);
    if (!inFile)
    {
        cerr << networkFileName << " file could not be opened" << endl;

        // Terminate the program with EXIT_FAILURE
        return 1;
    }

    string type;
    int id, parent_id;

    while (inFile >> type >> id >> parent_id)
    {
        if (type == "BS")
        {
            BaseStation *newBS = new BaseStation;
            newBS->child = NULL;
            newBS->counterPart = NULL;
            newBS->right = NULL;
            newBS->id = id;
            newBS->parent_id = parent_id;
            network.insertBS(newBS);
        }
        else
        {
            MobileHost *newMH = new MobileHost;
            newMH->right = NULL;
            newMH->id = id;
            newMH->parent_id = parent_id;
            network.insertMH(newMH);
        }
    }
    inFile.close();
    // READ NETWORK FILE END

    // READ MESSAGES FILE
    inFile.open(messagesFileName);
    if (!inFile)
    {
        cerr << messagesFileName << " file could not be opened" << endl;

        // Terminate the program with EXIT_FAILURE
        return 1;
    }
    string line;
    while (getline(inFile, line))
    {
        int positionOfSeperator = line.find('>');
        string target_id_string = line.substr(positionOfSeperator + 1);
        string content = line.substr(0, line.length() - (target_id_string.length() + 1));
        int target_id = stoi(target_id_string);

        Message *newMessage = new Message;
        newMessage->next = NULL;
        newMessage->content = content;
        newMessage->target_id = target_id;

        queue.enqueue(newMessage);
    }
    inFile.close();
    // READ MESSAGES FILE END

    network.processMessages(&queue, &stack);
    network.close();
    queue.close();
    stack.close();

    return 0;
}