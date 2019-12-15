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
    void close();
    void insertBS(BaseStation *);
    void insertMH(MobileHost *);
    void DFS_traverse(BaseStation *, int id);
    BaseStation *findBS(BaseStation *, int id);
    MobileHost *findMH(BaseStation *, int id);
    void processMessages(MessageQueue *);
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

void Network::DFS_traverse(BaseStation *BS, int id)
{
    cout << BS->id << " ";

    BaseStation *traverse = BS->child;
    while (traverse)
    {
        DFS_traverse(traverse, id);
        traverse = traverse->right;
    }

    // cout << BS->id << " ";
    // if (BS->child == NULL || BS->id == id)
    //     return;
    // BaseStation *traverse;
    // traverse = BS;
    // while (traverse)
    // {
    //     DFS_traverse(traverse->child, id);
    //     traverse = traverse->right;
    // }

    // if (BS == NULL)
    //     return;
    // if (BS->id == id)
    //     return BS;
    // else
    // {
    //     BaseStation *traverse;
    //     MobileHost *traverse2;
    //     traverse = BS;
    //     while (traverse)
    //     {
    //         cout << traverse->id << " | ";
    //         traverse2 = traverse->counterPart;
    //         while (traverse2)
    //         {
    //             cout << traverse2->id << " ";
    //             traverse2 = traverse2->right;
    //         }
    //         cout << endl;
    //         DFS_traverse(traverse->child);
    //         traverse = traverse->right;
    //     }
    // }
}

BaseStation *Network::findBS(BaseStation *BS, int id)
{
    if (BS && BS->id == id)
        return BS;
    else
    {
        BaseStation *traverse, *found;
        traverse = BS;
        while (traverse)
        {
            found = traverse;
            if (found->id == id)
                return found;
            found = findBS(traverse->child, id);
            if (found && found->id == id)
                return found;
            traverse = traverse->right;
        }
    }
}

void Network::insertMH(MobileHost *newMH)
{
    BaseStation *parentBS = findBS(centralController, newMH->parent_id);
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

MobileHost *Network::findMH(BaseStation *BS, int id)
{
    cout << BS->id << " ";
    MobileHost *MH = BS->getMobileHost(BS->counterPart, id);
    if (MH)
        return MH;

    BaseStation *traverse = BS->child;
    while (traverse)
    {
        MH = findMH(traverse, id);
        if (MH)
            return MH;
        traverse = traverse->right;
    }

    // MobileHost *MH = NULL;
    // BaseStation *traverse;
    // if (BS)
    // {
    //     MH = BS->getMobileHost(BS->counterPart, id);
    //     if (MH)
    //         return MH;
    // }
    // traverse = BS;
    // while (traverse)
    // {
    //     cout << traverse->id << " ";
    //     MH = traverse->getMobileHost(traverse->counterPart, id);
    //     if (MH)
    //         return MH;
    //     MH = findMH(traverse->child, id);
    //     if (MH)
    //         return MH;
    //     traverse = traverse->right;
    // }
}

void Network::processMessages(MessageQueue *queue)
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
            cout << "Can not be reached the mobile host mh_" << nextMessage->target_id << "at the moment" << endl;
        else
        {
            cout << "Message:" << nextMessage->content << endl;
        }
    }
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

int main(int argc, char *argv[])
{
    Network network;
    network.create();

    MessageQueue queue;
    queue.create();

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
    // READ MESSAGES FILE END

    network.processMessages(&queue);

    return 0;
}