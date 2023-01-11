
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#define chunksize (512 * 1024.0)
using namespace std;
void *dostuff(void *); /* function prototype */

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

class user
{
public:
    int userid;
    string password;
    int port;
    string ip;
    bool logged_in_or_not;
    user()
    {
        logged_in_or_not = false;
    }
};

class group
{
public:
    int groupid;
    int owner;
    vector<int> members;
    vector<string> files;
    vector<int> pendingreq;

    group()
    {
    }
};
class file
{
public:
    string file_name;
    unsigned long long int file_size;
    string sha1;
    int no_of_chunks;
    int groupid;
    string file_path;
    map<string, string> path_to_name;
    map<user *, int> which_user_how_manny_chunks;
};

map<string, vector<vector<user>>> which_chunk_which_peer;
map<int, user> userinfo;
map<int, group> groupinfo;
map<string, file> fileinfo;

// void colorgreen(string message)
// {
//     cout << "\033[0;32m";
//     cout << message;
//     cout << "\033[0m";
// }

void colorgreen(string message)
{
    cout << "\033[0;32m";
    cout << message;
    cout << "\033[0m";
}
void coloryellow(string message)
{
    cout << "\033[0;33m";
    cout << message;
    cout << "\033[0m";
}
void colorred(string message)
{
    cout << "\033[0;31m";
    cout << message;
    cout << "\033[0m";
}
void space_separated_words(string input, vector<string> &token)
{

    string word = "";
    for (auto x : input)
    {
        if (x == ' ')
        {
            token.push_back(word);
            word = "";
        }
        else
        {
            word = word + x;
        }
    }
    token.push_back(word);
}

string process_command(string x, vector<string> words, int userid_of_running_client)
{

    if (words[0] == "create_user")
    {
        // int id = stoi(words[1]);
        // string pass= words[2];
        // // string x = to_string(userid) + "$" + password;

        // user u;
        // u.password=pass;
        // u.userid=id;
        // userinfo[id]=u;
        // string x="User added";
        // return x;
        int id = stoi(words[1]);
        group g;
        if (userinfo.find(id) != userinfo.end())
        {
            string x = "User already created";
            return x;
        }
        // int id = stoi(words[1]);
        string pass = words[2];
        // string x = to_string(userid) + "$" + password;
        userid_of_running_client = stoi(words[1]);
        user u;
        u.password = pass;
        u.userid = id;
        userinfo[id] = u;
        string x = "User added";
        return x;

        // char * c = new char[buffer.length() + 1];
        // strcpy(c, buffer.c_str());
        // write(socket_desc, c, strlen(c));
        // bzero(c,256);
        // read(socket_desc,c,strlen(c));
    }
    else if (words[0] == "login")
    {

        int id = stoi(words[1]);
        string pass = words[2];

        if (userinfo.find(id) == userinfo.end())
        {
            string x = "User not created";
            return x;
        }
        else
        {
            if (userinfo[id].password == pass)
            {
                string x = "User registered";
                userinfo[id].logged_in_or_not = true;
                // cout << userinfo.empty();
                // for (auto it = userinfo.begin(); it != userinfo.end(); it++)
                // {
                //     cout << it->first << endl;
                //     cout << it->second.password << endl;
                //     // for(auto x:it->second.){
                //     //     cout<<x;
                //     // }
                // }
                return x;
            }
            else
            {
                string x = "Wrong Password";
                return x;
            }
        }
    }
    else if (words[0] == "create_group")
    {

        int id = stoi(words[1]);
        group g;
        if (groupinfo.find(id) != groupinfo.end())
        {
            string x = "Group already created";
            return x;
        }
        if (userinfo.find(userid_of_running_client) == userinfo.end())
        {
            string x = "You cannot create a group as you are not a user";
            return x;
        }
        if (userinfo[userid_of_running_client].logged_in_or_not == false)
        {
            string x = "You cannot create a group as you are not logged in.";
            return x;
        }
        g.groupid = id;
        // cout << userid_of_running_client << "hello" << endl;
        g.members.push_back(userid_of_running_client);
        // g.members.push_back(userid_of_running_client);
        // g.members.push_back(userid_of_running_client);
        // cout << "bye" << endl;
        // cout << g.members[0];

        g.owner = userid_of_running_client;
        groupinfo[id] = g;
        // cout << userinfo.empty();
        for (auto it = groupinfo.begin(); it != groupinfo.end(); it++)
        {
            cout << it->first << "->" << endl;
            // cout << it->second.password << endl;
            // for(auto x:it->second.members){
            //     cout<<x<<" ";
            // }
            // for (int i = 0; i < it->second.members.size(); i++)
            // {
            //     cout << "members are " << it->second.members[i] << endl;
            // }
        }

        string x = "Group created successfully";
        return x;

        //  int id = stoi(words[1]);
        // string pass= words[2];

        // else{
        //     if(userinfo[id].password==pass){
        //         string x="User registered";
        //         return x;
        //     }
        //     else{
        //         string x="Wrong Password";
        //         return x;
        //     }
        // }
    }

    else if (words[0] == "join_group")
    {

        int id = stoi(words[1]);

        if (groupinfo.find(id) == groupinfo.end())
        {
            string x = "Group not created";
            return x;
        }
        else if (userinfo.find(userid_of_running_client) == userinfo.end())
        {
            string x = "You cannot join a group as you are not a user";
            return x;
        }
        else if (userinfo[userid_of_running_client].logged_in_or_not == false)
        {
            string x = "You cannot join a group as you are not logged in.";
            return x;
        }
        else
        {
            groupinfo[id].pendingreq.push_back(userid_of_running_client);
            string x = "Request to join the group sent successfully";
            return x;
        }
    }

    else if (words[0] == "leave_group")
    {

        int id = stoi(words[1]);

        if (groupinfo.find(id) == groupinfo.end())
        {
            string x = "Group not created";
            return x;
        }
        else if (userinfo.find(userid_of_running_client) == userinfo.end())
        {
            string x = "You cannot leave a group as you are not a user";
            return x;
        }
        else if (userinfo[userid_of_running_client].logged_in_or_not == false)
        {
            string x = "You cannot leave a group as you are not logged in.";
            return x;
        }
        else
        {
            // remove(groupinfo[id].members.begin(), groupinfo[id].members.end(), userid_of_running_client);
            auto it = find(groupinfo[id].members.begin(), groupinfo[id].members.end(), userid_of_running_client);
            if (it != groupinfo[id].members.end())
            {
                groupinfo[id].members.erase(it);
                string x = "Group left successfully";
                //     for (auto x : groupinfo[id].members)
                // {
                //     cout << groupinfo[id].groupid << "->";
                //     cout << x << " ";
                // }

                // for (auto it = groupinfo.begin(); it != groupinfo.end(); it++)
                // {
                //     cout << it->first << " -> ";
                //     // cout<<it->second<<endl;
                //     for (auto x : it->second.members)
                //     {
                //         cout << x << " ";
                //     }
                //     cout << endl;
                // }
                return x;
            }
            string x = "You are not the member of the  group";
            return x;
        }
    }
    else if (words[0] == "requests")
    {

        int id = stoi(words[2]);

        if (groupinfo.find(id) == groupinfo.end())
        {
            string x = "Group not created";
            return x;
        }
        else if (groupinfo[id].pendingreq.size() == 0)
        {
            string x = "NO pending requests as of now.....";
            return x;
        }
        else
        {
            // remove(groupinfo[id].members.begin(),groupinfo[id].members.end(),userid_of_running_client);

            string x = "";
            // for (auto it = groupinfo.begin(); it != groupinfo.end(); it++)
            // {
            //     cout << it->first << " -> ";
            //     // cout<<it->second<<endl;
            //     for (auto x : it->second.pendingreq)
            //     {
            //         cout << x << " ";
            //     }
            //     cout << endl;
            // }
            for (auto y : groupinfo[id].pendingreq)
            {
                x += to_string(y) + ",";
                // cout<<"User with userid "<<x<<" has made a request."<<endl;
            }
            return x;
        }
    }

    else if (words[0] == "accept_request")
    {
        int gr_id = stoi(words[1]);
        int user_id = stoi(words[2]);

        if (userinfo.find(user_id) == userinfo.end())
        {
            string x = "User not created";
            return x;
        }
        else if (groupinfo.find(gr_id) == groupinfo.end())
        {
            string x = "Group not created";
            return x;
        }
        else if (userinfo[user_id].logged_in_or_not == false)
        {
            string x = "You cannot join a group as you are not logged in.";
            return x;
        }
        else if (find(groupinfo[gr_id].pendingreq.begin(), groupinfo[gr_id].pendingreq.end(), user_id) == groupinfo[gr_id].pendingreq.end())
        {
            string x = "You haven't requested the group to be a part of it.";
            return x;
        }
        else if (groupinfo[gr_id].owner != userid_of_running_client)
        {
            string x = "You cannot accept the request for this group as you are not the admin of this group.";
            return x;
        }
        else
        {
            // remove(groupinfo[id].members.begin(),groupinfo[id].members.end(),userid_of_running_client);

            // string x = "";
            // for (auto it = groupinfo.begin(); it != groupinfo.end(); it++)
            // {
            //     cout << it->first <<" -> ";
            //     // cout<<it->second<<endl;
            //     for (auto x : it->second.pendingreq)
            //     {
            //         cout << x<<" ";
            //     }
            //     cout<<endl;
            // }
            // for (auto y : groupinfo[id].pendingreq)
            // {
            //     x += to_string(y) + ",";
            //     // cout<<"User with userid "<<x<<" has made a request."<<endl;

            // }
            // return x;

            groupinfo[gr_id].members.push_back(user_id);
            // remove(groupinfo[gr_id].pendingreq.begin(),groupinfo[gr_id].pendingreq.end(),user_id);
            auto it = find(groupinfo[gr_id].pendingreq.begin(), groupinfo[gr_id].pendingreq.end(), user_id);
            if (it != groupinfo[gr_id].pendingreq.end())
            {
                groupinfo[gr_id].pendingreq.erase(it);
            }
            // for (auto x : a)
            // {
            //     cout << x << " ";
            // }
            // for (auto x : groupinfo[gr_id].pendingreq)
            // {
            //     cout << groupinfo[gr_id].groupid << "->";
            //     cout << x << " ";
            // }
            // for (auto it = groupinfo.begin(); it != groupinfo.end(); it++)
            // {
            //     cout << it->first << " -> ";
            //     // cout<<it->second<<endl;
            //     for (auto x : it->second.pendingreq)
            //     {
            //         cout << x << " ";
            //     }
            //     cout << endl;
            // }
            cout << endl;
            cout << "=====================================================================" << endl;
            // for (auto x : groupinfo[gr_id].members)
            // {
            //     cout << groupinfo[gr_id].groupid << "->";
            //     cout << x << " ";
            // }

            // for (auto it = groupinfo.begin(); it != groupinfo.end(); it++)
            // {
            //     cout << it->first << " -> ";
            //     // cout<<it->second<<endl;
            //     for (auto x : it->second.members)
            //     {
            //         cout << x << " ";
            //     }
            //     cout << endl;
            // }
            string x = "User with userid " + to_string(user_id) + " has been added to the group";
            return x;
        }
    }

    else if (words[0] == "list_groups\n")
    {

        if (groupinfo.size() == 0)
        {
            string x = "Groups not created";
            return x;
        }

        // else
        // {
        // remove(groupinfo[id].members.begin(),groupinfo[id].members.end(),userid_of_running_client);
        // for (auto it = groupinfo.begin(); it != groupinfo.end(); it++)
        // {
        //     cout << it->first << " -> ";
        //     // cout<<it->second<<endl;
        // }
        string x = "";
        for (auto it = groupinfo.begin(); it != groupinfo.end(); it++)
        {
            x += to_string(it->first) + ",";
        }

        return x;
    }

    else if (words[0] == "list_files")
    {
        int id = stoi(words[1]);
        int peerid = stoi(words[2]);
        if (groupinfo.find(id) == groupinfo.end())
        {
            string x = "$Group not created";
            return x;
        }
        else if (std::count(groupinfo[id].members.begin(), groupinfo[id].members.end(), peerid) == 0)
        {
            //     // userinfo[userid_of_running_client].logged_in_or_not=false;
            //     string x="You haved logged out.";
            //     return x;
            // }
            // else{
            string x = "$You are not a part of the group..";
            return x;
        }
        else if (groupinfo[id].files.size() == 0)
        {
            string x = "$No files uploaded till now....";
            return x;
        }
        // else
        // {
        // remove(groupinfo[id].members.begin(),groupinfo[id].members.end(),userid_of_running_client);
        // for (auto it = groupinfo.begin(); it != groupinfo.end(); it++)
        // {
        //     cout << it->first << " -> ";
        //     // cout<<it->second<<endl;
        // }
        string x = "";
        for (auto i : groupinfo[id].files)
        {
            x += i + ",";
        }

        return x;
    }

    else if (words[0] == "logout\n")
    {

        if (userinfo[userid_of_running_client].logged_in_or_not == true)
        {
            userinfo[userid_of_running_client].logged_in_or_not = false;
            string x = "You haved logged out.";
            return x;
        }
        else
        {
            string x = "You haved not logged in only.first login and then come ..Okay...";
            return x;
        }
    }

    else if (words[0] == "stop_sharing")
    {

        int id = stoi(words[1]);
        string file_path = words[2];
        file_path = file_path.substr(0, file_path.length() - 1);
        cout << "Hello in stop sharing" << endl;
        for (auto x : groupinfo[id].files)
        {
            cout << groupinfo[id].groupid << "->";
            cout << x << " ";
        }
        cout << "THE FILE PATH IS:" << file_path;
        // return file_path;
        if (groupinfo.find(id) == groupinfo.end())
        {
            string x = "Group not created";
            return x;
        }
        // else if (userinfo.find(userid_of_running_client) == userinfo.end())
        // {
        //     string x = "You cannot leave a group as you are not a user";
        //     return x;
        // }
        else if (find(groupinfo[id].files.begin(), groupinfo[id].files.end(), file_path) == groupinfo[id].files.end())
        {
            string x = "There is no such file in this group.";
            return x;
        }
        else
        {
            // remove(groupinfo[id].members.begin(), groupinfo[id].members.end(), userid_of_running_client);
            auto it = find(groupinfo[id].files.begin(), groupinfo[id].files.end(), file_path);
            cout << "The fie path is : " << file_path;
            if (it != groupinfo[id].files.end())
            {
                groupinfo[id].files.erase(it);
                string x = "Stopped sharing  successfully";

                return x;
            }
        }
    }

    else if (words[0] == "upload_file")
    {
        string file_path = words[6];
        string sha1 = words[8];

        int gr_id = stoi(words[1]);
        int peerid = stoi(words[5]);
        // cout << "gr_id is: " << gr_id << endl;

        string filename = words[2];
        // cout << "filename is: " << filename << endl;
        unsigned long long int filesize = stoi(words[3]);
        // cout << "filesizee is: " << filesize << endl;

        if (groupinfo.find(gr_id) == groupinfo.end())
        {
            string x = "Group not created";
            return x;
        }

        else if (std::count(groupinfo[gr_id].members.begin(), groupinfo[gr_id].members.end(), peerid) == 0)
        {
            //     // userinfo[userid_of_running_client].logged_in_or_not=false;
            //     string x="You haved logged out.";
            //     return x;
            // }
            // else{
            string x = "You are not a part of the group..";
            return x;
        }
        else if (std::count(groupinfo[gr_id].files.begin(), groupinfo[gr_id].files.end(), file_path))
        {
            //     // userinfo[userid_of_running_client].logged_in_or_not=false;
            //     string x="You haved logged out.";
            //     return x;
            // }
            // else{
            string x = "File already in the group..";
            return x;
        }
        else
        {
            // cout<<"This is the checkpoint 1"<<endl;
            file f;
            f.file_name = filename;
            f.file_size = filesize;
            f.sha1 = sha1;
            f.groupid = gr_id;
            f.no_of_chunks = ceil(filesize / chunksize);
            f.file_path = file_path;
            f.path_to_name[file_path] = filename;
            groupinfo[gr_id].files.push_back(file_path);
            for (auto x : groupinfo[gr_id].files)
            {
                cout << groupinfo[gr_id].groupid << "->";
                cout << x << " ";
            }
            cout << "This is the checkpoint 2" << endl;
            user u;
            u.port = stoi(words[4]);
            //  cout<<"This is the checkpoint 3"<<endl;
            u.userid = stoi(words[5]);
            //  cout<<"This is the checkpoint 4"<<endl;
            string file_path = words[6];
            //  cout<<"This is the checkpoint 5"<<endl;
            f.which_user_how_manny_chunks[&userinfo[u.userid]] = f.no_of_chunks;
            //  cout<<"This is the checkpoint 6"<<endl;
            fileinfo[file_path] = f;
            //   cout<<"This is the checkpoint 7"<<endl;
            vector<vector<user>> vu(f.no_of_chunks);
            which_chunk_which_peer[f.file_path] = vu;
            for (int i = 0; i < f.no_of_chunks; i++)
            {

                //  peers.push_back(u);
                //   cout<<"This is the checkpoint final"<<endl;
                which_chunk_which_peer[f.file_path][i].push_back(u);
            }
            //   cout<<"This is the checkpoint 8"<<endl;
            string x = "File uploaded successfully";
            return x;
        }
    }

    else if (words[0] == "download_file")
    {
        cout << "Inthe download of tracker" << endl;
        string dest_path = words[6];
        int gr_id = stoi(words[1]);
        int peerid = stoi(words[5]);
        cout << "gr_id is: " << gr_id << endl;
        string file_path = words[4];

        string filename = words[2];
        cout << "filename is: " << filename << endl;
        unsigned long long int filesize = stoi(words[3]);
        cout << "filesizee is: " << filesize << endl;

        if (groupinfo.find(gr_id) == groupinfo.end())
        {
            string x = "Group not created";
            return x;
        }

        else if (std::count(groupinfo[gr_id].members.begin(), groupinfo[gr_id].members.end(), peerid) == 0)
        {
            //     // userinfo[userid_of_running_client].logged_in_or_not=false;
            //     string x="You haved logged out.";
            //     return x;
            // }
            // else{
            string x = "You are not a part of the group..";
            return x;
        }
        else if (std::count(groupinfo[gr_id].files.begin(), groupinfo[gr_id].files.end(), file_path) == 0)
        {
            //     // userinfo[userid_of_running_client].logged_in_or_not=false;
            //     string x="You haved logged out.";
            //     return x;
            // }
            // else{
            string x = "File not uploaded in th egroup..";
            return x;
        }
        else
        {
            // file f;
            // f.file_name = filename;
            // f.file_size = filesize;
            // f.groupid = gr_id;
            // f.no_of_chunks = ceil(filesize / chunksize);
            // f.file_path = file_path;

            // fileinfo[filename] = f;
            // user u;
            // u.port = stoi(words[4]);
            // u.userid = stoi(words[5]);
            // string file_path = words[6];

            // for (int i = 0; i < f.no_of_chunks; i++)
            // {

            //     //  peers.push_back(u);
            //     which_chunk_which_peer[f.file_name][i].push_back(u);
            // }
            // cout << "hello atleast reached here" << endl;
            string no_of_chunks;
            no_of_chunks = to_string(fileinfo[file_path].no_of_chunks);
            string x = "%";
            return (x + " " + no_of_chunks + " " + fileinfo[file_path].sha1);
        }
    }
    else
    {
        string x = "INVALID COMMAND !!!! ..TRY AGAIN......";
        return x;
    }
}

void *callquit(void *p)
{

    string status;
    cin >> status;
    if (status == "quit")
    {
        coloryellow( "::::::::::::::::::THANK YOU:::::::::::::::::::::::::");
        cout << endl;
        coloryellow( "::::::::::::YOU HAVE MADE AN EXIT:::::::::::::::::::");
        cout << endl;
        exit(1);
    }
    return NULL;
}
int main(int argc, char *argv[])
{
    cout << endl;
    coloryellow ("............WELCOME TO THE PEER TO PEER FILE SHARING SYSTEM........." );
    cout<< endl;
    cout << endl;
    coloryellow( "............YOU ARE VIEWING THE TRACKER SIDE TERMINAL........." );
    cout<< endl;
    cout << endl;
    int sockfd, newsockfd, portno, pid;
    int *new_sock;
    char *message;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no info provided\n");
        exit(1);
    }

    fstream newfile;
    newfile.open(argv[1], ios::in);

    if (newfile.is_open())
    {
        string tp;

        getline(newfile, tp);
        // cout << tp;
        portno = atoi(tp.c_str());

        newfile.close();
    }

    //create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    //  portno = atoi(argv[1]);
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse)) < 0)
        error("setsockopt(SO_REUSEADDR) failed");

    //Prepare the sockaddr_in structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = portno;

    //bind
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    //listen
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    pthread_t t;
    pthread_create(&t, NULL, callquit, NULL);
    //accepting connections in an infinite loop
    while (1)
    {

        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

        if (newsockfd < 0)
        {
            error("ERROR on accept");
            return 1;
        }

        pthread_t sniffer_thread;
        new_sock = (int *)malloc(1);
        *new_sock = newsockfd;

        if (pthread_create(&sniffer_thread, NULL, dostuff, (void *)new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }

        // pthread_join(sniffer_thread, NULL);

        // close(newsockfd);
    } /* end of while */
    close(sockfd);
    return 0;
}

void *dostuff(void *socket_desc)
{
    int sock = *(int *)socket_desc;
    int n;
    char *message, client_message[2000];
    char buffer[256];
    int userid_of_running_client;
    // send(sock, "Hello, world!\n", 13, 0);
    while (true)
    {
        bzero(buffer, 256);

        n = read(sock, buffer, 255);
        if (n < 0)
            error("ERROR reading from socket");
        printf("Message from the client: %s", buffer);
        vector<string> words;
        space_separated_words(buffer, words);
        // cout << "Fromo client: " << buff << endl;
        if (words[0] == "create_user")
        {
            userid_of_running_client = stoi(words[1]);
        }
        for (auto x : words)
        {
            cout << x << endl;
        }

        if (words[0] == "upload_file")
        {
            for (auto x : words)
            {
                cout << x << endl;
            }
        }
        string x = process_command(buffer, words, userid_of_running_client);
        if (x[0] == '%')
        {
            char *buff1 = new char[x.length() + 1];
            strcpy(buff1, x.c_str());
            // write(socket_desc, c, strlen(c));
            bzero(buffer, 256);
            write(sock, buff1, strlen(buff1));
            bzero(buffer, 256);

            //========================Running a for loop in tracker  ============================================

            int no_chunk = stoi(x.substr(1));
            for (int i = 0; i < no_chunk; i++)
            {
                bzero(buffer, 256);
                cout << "Entering the for loop " << i + 1 << " time" << endl;
                n = read(sock, buffer, 255);
                printf("Message from the client: %s\n", buffer);

                //=======================  logic for peer selection algorithm ====================================

                vector<string> store;
                space_separated_words(buffer, store);
                string file_path_for_download = store[0];
                int chunk_no_for_download = stoi(store[1]);
                cout << "The chunk for download is : " << chunk_no_for_download << endl;
                vector<user> list_of_peers_having_that_chunk = which_chunk_which_peer[file_path_for_download][chunk_no_for_download];
                user peer_with_min_chunks;
                int minpieces = 100000;
                for (auto i : list_of_peers_having_that_chunk)
                {
                    if (minpieces > fileinfo[file_path_for_download].which_user_how_manny_chunks[&i])
                    {
                        minpieces = fileinfo[file_path_for_download].which_user_how_manny_chunks[&i];
                        peer_with_min_chunks = i;
                    }
                    //  pieces=min(pieces,fileinfo[file_path_for_download].which_user_how_manny_chunks[i]);
                }

                //========================Sendimg  the reply thar which peer is selected to the peer====================
                string ip = "127.0.0.1";
                x = ip + " " + to_string(peer_with_min_chunks.port) + " " + file_path_for_download + " " + store[1];
                char *buff1 = new char[x.length() + 1];
                strcpy(buff1, x.c_str());
                // write(socket_desc, c, strlen(c));
                bzero(buffer, 256);
                write(sock, buff1, strlen(buff1));

                //=============================================idhar tak pehle check karo =======================================
                bzero(buffer, 256);

                n = read(sock, buffer, 255);
                vector<string> store1;
                space_separated_words(buffer, store1);
                string filepath_for_updation = store1[0];
                int chunkno_for_updation = stoi(store1[1]);
                int peerid_for_updation = stoi(store1[2]);
                fileinfo[filepath_for_updation].which_user_how_manny_chunks[&userinfo[peerid_for_updation]]++;
                which_chunk_which_peer[filepath_for_updation][chunkno_for_updation].push_back(userinfo[peerid_for_updation]);
            }
        }

        //===============================================================================================
        else
        {
            char *buff1 = new char[x.length() + 1];
            strcpy(buff1, x.c_str());
            // write(socket_desc, c, strlen(c));
            bzero(buffer, 256);
            write(sock, buff1, strlen(buff1));
        }
    }

    //Free the socket pointer
    free(socket_desc);

    return 0;
}
