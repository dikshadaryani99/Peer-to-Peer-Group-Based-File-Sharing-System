// #include <bits/stdc++.h>
// #include <termios.h>
// #include <sys/ioctl.h>
// #include <unistd.h>
// #include <sys/stat.h>
// #include <dirent.h>
// #include <grp.h>
// #include <pwd.h>
// #include <sys/wait.h>
// #include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fstream>
#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <pthread.h>
#include<openssl/sha.h>

#define chunksize (512 * 1024.0)
using namespace std;
int keep_count = 0;
std::ofstream fout;
void *client_as_server(void *);
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
string pathtoname(string file_path){
    int index;
            for (int i = 0; i <= file_path.length(); i++)
            {
                if (file_path[i] == '/')
                {
                    index = i;
                }
            }
            string file_name = file_path.substr(index + 1);
            return file_name;
}


inline bool exists(const std::string &filename)
{
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

// vector<vector<user>> list_of_peers;
void *communication_with_peer(void *);
vector<string> split(string str, char del)
{
    vector<string> v;
    // declaring temp string to store the curr "word" upto del
    string temp = "";

    for (int i = 0; i < (int)str.size(); i++)
    {
        // If cur char is not del, then append it to the cur "word", otherwise
        // you have completed the word, print it, and start a new word.
        if (str[i] != del)
        {
            temp += str[i];
        }
        else
        {
            // cout << temp << " ";
            v.push_back(temp);
            temp = "";
        }
    }
    v.push_back(temp);
    return v;
}
string convertToString(char *a, int size)
{
    string s = a;
    return s;
}


string calculatesha(string filename)
{
    SHA256_CTX context;
    if (!SHA256_Init(&context))
    {
        return "false";
    }
    static const int K_READ_BUF_SIZE{1024 * 16};
    char buf[K_READ_BUF_SIZE];
    std::ifstream file(filename, std::ifstream::binary);
    while (file.good())
    {
        file.read(buf, sizeof(buf));
        if (!SHA256_Update(&context, buf, file.gcount()))
        {
            return "false";
        }
    }

    unsigned char result[SHA256_DIGEST_LENGTH];
    if (!SHA256_Final(result, &context))
    {
        return "false";
    }

    std::stringstream shastr;
    shastr << std::hex << std::setfill('0');
    for (const auto &byte : result)
    {
        shastr << std::setw(2) << (int)byte;
    }
    return shastr.str();
    // cout<<shastr.str()<<endl;
}


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
    bool loggedin;
    vector<int> ownerofwhichgroups;
};

class group
{
public:
    int groupid;
    int owner;
    vector<int> members;
    vector<string> files;
    vector<int> pendingreq;
};
class file
{
public:
    string file_name;
    unsigned long long int file_size;
    string sha1;
    int no_of_chunks;
    int groupid;
    map<int, string> chunk_and_data;
};


class download_details
{
    public:
    int grid;
    int peerid;
    string status;
    download_details(){}
};

map<string,download_details> downloads_dikhao;


map<string, vector<vector<user>>> which_chunk_which_peer;
map<string, file> fileinformation;
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
void comma_separated_words(string input, vector<string> &token)
{

    string word = "";
    for (auto x : input)
    {
        if (x == ',')
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

void *ask_chunk(void *ch1)
{

    string info = (char *)ch1;
    vector<string> information_for_peer;
    space_separated_words(info, information_for_peer);
    string ip = information_for_peer[0];
    cout << "The ip is: " << ip << endl;
    int port = stoi(information_for_peer[1]);
    cout << "The port is: " << port << endl;
    string filepath = information_for_peer[2];
    cout << "The filepath is: " << filepath << endl;
    string chunk_for_download = information_for_peer[3];
    cout << "The chunk is: " << chunk_for_download << endl;
    string destination_path = information_for_peer[4];
    cout << "The destination is: " << destination_path << endl;
    int total_chunks_of_the_file = stoi(information_for_peer[5]);

    int socket_peer, n, portno;
    struct sockaddr_in server;
    char *message, server_reply[2000];
    // char buffer[256];
    socket_peer = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_peer == -1)
    {
        printf("Could not create socket");
    }

    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = port;

    //Connect to remote server
    if (connect(socket_peer, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        puts("connect error");
    }

    puts("Connected in download command");

    string buffer = filepath + " " + chunk_for_download;
    char *c = new char[buffer.length() + 1];
    strcpy(c, buffer.c_str());
    write(socket_peer, c, strlen(c));
    // bzero(c,256);
    char ch[512 * 1024];
    bzero(ch, 512 * 1024);

    //===========================================================================
    // int bytesread = 0;
    // int x=512*1024;
    // int result;
    // while (bytesread != x && (stoi(chunk_for_download)!= total_chunks_of_the_file-1))
    // {
    //     result = read(socket_peer, ch, x - bytesread);
    //     if (result < 1 )
    //     {
    //         cout<<"error";
    //     }

    //     bytesread += result;
    // }

    // if((stoi(chunk_for_download)== total_chunks_of_the_file-1)){
    //      bytesread=read(socket_peer, ch, 512*1024);

    // }
    // cout<<"THe SIze od date read is: "<<bytesread<<endl;

    //     int numread;
    //     int byteread=0;
    // while(1) {
    //     if ((numread = read(socket_peer, ch, sizeof(ch)) == -1)){

    //         perror("read");
    //         exit(1);
    //     }
    //     cout<<"Hi i am in the while loop"<<endl;
    //     byteread+=numread;
    //     if (numread == 0)
    //        break;

    // }
    //  int numread;
    //     int byteread=0;
    // while(1) {
    //     if ((numread = read(socket_peer, ch, sizeof(ch)) == -1)){

    //         perror("read");
    //         exit(1);
    //     }
    //     cout<<"Hi i am in the while loop"<<endl;
    //     byteread+=numread;
    //     if (numread == 0)
    //        break;

    // }

    char sizechunk[1024];
    recv(socket_peer, sizechunk, sizeof(sizechunk), 0);
    // cout << "The size of the data written by the peer : " << sizechunk << endl;
    string sizechu(sizechunk);
    int databytes = stoi(sizechu);
    keep_count++;
    cout << "databytes to be received " << databytes << endl;


     vector<char>chunkrakhahaiyaha(databytes);
     int count=0;
    while(count<databytes-1)
    {
        int n = recv(socket_peer,&chunkrakhahaiyaha[count],chunkrakhahaiyaha.size()-count,0);
        if (n <=0)
            break;
        count+=n;
    }

    // char chunkdata[512 * 1024];
    // int count = 0;
    // while (count <databytes - 1)
    // {
    //     int n = recv(socket_peer, chunkdata + count, sizeof(chunkdata) - count, 0);
    //     // cout << "The byters read in the while loop are foe chunk " << chunk_for_download << ": " << n << endl;
    //     // cout<<"Total data read till now is : "<<count<<endl;
    //     if (n <= 0)
    //         break;
    //     count += n;
    // }
    string s(chunkrakhahaiyaha.begin(),chunkrakhahaiyaha.end());
    //====================================================================================
    // int k=recv(socket_peer, ch, 512*1024,0);
    cout << "THe SIze of data read is: " << count << endl;

    // cout<<"Buffer len is "<<strlen(chunkdata)<<endl;
    cout<<"SSSSSSSSSSString size is "<<s.length()<<endl;

    //==================================================================================
    
    fout.seekp(stoi(chunk_for_download) * 512 * 1024, fout.beg);
    fout << s;
     fout.seekp(stoi(chunk_for_download) * 512 * 1024, fout.beg);

     

    // string buffer = filepath+" "+chunk_for_download+" "+to_string(myowninfo.userid);
    // char *c = new char[buffer.length() + 1];
    // strcpy(c, buffer.c_str());
    // write(socket_desc, c, strlen(c));
    // if (keep_count < total_chunks_of_the_file)
    // {
    //     // cout<<"Hi ready to read the file"<<endl;
    //     fstream obj;
    //     obj.open(destination_path, ios::out | ios::app);
    //     obj.close();
    //     obj.open(destination_path, ios::out | ios::in);
    //     obj.seekp(stoi(chunk_for_download) * 512 * 1024);
    //     obj << chunkdata;
    //     obj.close();
    // }
    // if(ch[0]!='G'){

    // vector<string> token;
    // comma_separated_words(ch, token);
    // token.pop_back();
    // printf("the message from server is that the  are :%s\n", ch);
}


int my_userid;
string my_password;
user myowninfo;
void checkcommand(int socket_desc, vector<string> words, string buffer, pthread_t run_as_server)
{
    if (words[0] == "create_user")
    {
        // cout << "hi";
        myowninfo.userid = stoi(words[1]);
        myowninfo.password = words[2];
        myowninfo.port = stoi(words[words.size() - 1]);
        // string x = to_string(userid) + "$" + password;
        buffer = buffer + " " + words[words.size() - 1];
        char *c = new char[buffer.length() + 1];
        strcpy(c, buffer.c_str());
        write(socket_desc, c, strlen(c));
        // bzero(c,256);
        char ch[256];
        bzero(ch, 256);
        read(socket_desc, ch, 256);
        printf("The message from server is: %s ", ch);
        cout<<endl;
        cout<<endl;
    }
    else if (words[0] == "login")
    {

        char *c = new char[buffer.length() + 1];
        strcpy(c, buffer.c_str());
        write(socket_desc, c, strlen(c));
        // bzero(c,256);
        char ch[256];
        bzero(ch, 256);
        read(socket_desc, ch, 256);
        printf("the message from server is: %s ", ch);
        cout<<endl;
        cout<<endl;
    }
    else if (words[0] == "create_group")
    {

        int id = stoi(words[1]);
        char *c = new char[buffer.length() + 1];
        strcpy(c, buffer.c_str());
        write(socket_desc, c, strlen(c));
        // bzero(c,256);
        char ch[256];
        bzero(ch, 256);
        read(socket_desc, ch, 256);
        if (ch == "Group created successfully")
        {
            myowninfo.ownerofwhichgroups.push_back(id);
        }
        printf("the message from server is: %s ", ch);
        cout<<endl;
        cout<<endl;
    }
    else if (words[0] == "join_group")
    {

        char *c = new char[buffer.length() + 1];
        strcpy(c, buffer.c_str());
        write(socket_desc, c, strlen(c));
        // bzero(c,256);
        char ch[256];
        bzero(ch, 256);
        read(socket_desc, ch, 256);
        printf("the message from server is: %s ", ch);
        cout<<endl;
        cout<<endl;
    }
    else if (words[0] == "leave_group")
    {

        char *c = new char[buffer.length() + 1];
        strcpy(c, buffer.c_str());
        write(socket_desc, c, strlen(c));
        // bzero(c,256);
        char ch[256];
        bzero(ch, 256);
        read(socket_desc, ch, 256);
        printf("the message from server is: %s ", ch);
        cout<<endl;
        cout<<endl;
    }
    else if (words[0] == "requests")
    {

        int id = stoi(words[2]);
        char *c = new char[buffer.length() + 1];
        strcpy(c, buffer.c_str());
        write(socket_desc, c, strlen(c));
        // bzero(c,256);
        char ch[256];
        bzero(ch, 256);
        read(socket_desc, ch, 256);
        if (ch[0] != 'G')
        {

            vector<string> token;
            comma_separated_words(ch, token);
            token.pop_back();
            printf("the message from server is that  the requests for  the group %d : \n", id);
            for (auto y : token)
            {

                cout << "User with userid " << y << " has made a request." << endl;
            }
            cout<<endl;
        cout<<endl;
        }
        else
        {
            printf("the message from server is: %s ", ch);
            cout<<endl;
        cout<<endl;
        }
    }
    else if (words[0] == "accept_request")
    {

        char *c = new char[buffer.length() + 1];
        strcpy(c, buffer.c_str());
        write(socket_desc, c, strlen(c));
        // bzero(c,256);
        char ch[256];
        bzero(ch, 256);
        read(socket_desc, ch, 256);
        printf("the message from server is: %s ", ch);
        cout<<endl;
        cout<<endl;
    }
    else if (words[0] == "logout\n")
    {

        char *c = new char[buffer.length() + 1];
        strcpy(c, buffer.c_str());
        write(socket_desc, c, strlen(c));
        // bzero(c,256);
        char ch[256];
        bzero(ch, 256);
        read(socket_desc, ch, 256);
        printf("the message from server is: %s ", ch);
        cout<<endl;
        cout<<endl;
    }
    else if (words[0] == "list_groups\n")
    {

        char *c = new char[buffer.length() + 1];
        strcpy(c, buffer.c_str());
        write(socket_desc, c, strlen(c));
        // bzero(c,256);
        char ch[256];
        bzero(ch, 256);
        read(socket_desc, ch, 256);
        if(ch[0]!='G'){

        vector<string> token;
        comma_separated_words(ch, token);
        token.pop_back();
        // if(token.size()==0){
        printf("the message from server is that the groups present are :\n");
        for (auto y : token)
        {

            cout << "Group " << y << endl;
        }
        
        
        }
        else{
            
             printf("the message from server is: %s ",ch);
        }
        cout<<endl;
        cout<<endl;
    }
    else if (words[0] == "list_files")
    {
        int gr_id = stoi(words[1]);
        string d = "list_files";
        string to_tracker = d + " " + to_string(gr_id) + " " + to_string(myowninfo.userid);
        cout<<"THe id of the user is: "<<myowninfo.userid;
        char *c = new char[to_tracker.length() + 1];
        strcpy(c, to_tracker.c_str());
        write(socket_desc, c, strlen(c));
        // bzero(c,256);
        char ch[256];
        bzero(ch, 256);
        read(socket_desc, ch, 256);
        if(ch[0]!='$'){

        vector<string> token;
        comma_separated_words(ch, token);
        token.pop_back();
        printf("the message from server is that the files present in the group are :\n");
        for (auto y : token)
        {

            cout << "File :  " << y << endl;
        }
        }
        else{
            string say=ch;
            say=say.substr(1);
             printf("the message from server is: %s ",say.c_str());
        }
        cout<<endl;
        cout<<endl;
    }
    else if (words[0] == "upload_file")
    {
        file f;

        string gr_id = words[2];
        string file_name;
        string file_path = words[1];
        char cha = '/';
        string cur_dir_path;
        int count = std::count(file_path.begin(), file_path.end(), cha);
        if (count == 0)
        {
            file_name = file_path;
            char cwd[256];
            if (getcwd(cwd, sizeof(cwd)) == NULL)
                perror("getcwd() error");
            else
                cur_dir_path = cwd;
            file_path = cur_dir_path + "/" + file_name;
            // cout << "the cwd is : " << cur_dir_path << endl;
        }
        else
        {
            int index;
            for (int i = 0; i <= file_path.length(); i++)
            {
                if (file_path[i] == cha)
                {
                    index = i;
                }
            }
            file_name = file_path.substr(index + 1);
        }

        //         ifstream ifile;
        //    ifile.open(file_path);
        //    if(ifile) {
        //       cout<<"file exists";
        //    } else {
        //       cout<<"file doesn't exist";
        //    }
        // cout << "th e file path is: " << file_path << endl;
        // if (exists(file_path))
        // {
        //     cout << "File exists ";
        // }
        // else
        // {
        //     cout << "File doesnt exist";
        // }

        // cout << "The filename is: " << file_name << endl;
        string file_size;
        ifstream ifs(file_name);
        ifs.seekg(0, ios::end);
        unsigned long long int sizeInBytes = ifs.tellg();
        ifs.close();

        //   long GetFileSize(std::string filename)
        // cout << "The file size is " << sizeInBytes << endl;
        struct stat stat_buf;
        int rc = stat(file_path.c_str(), &stat_buf);
        rc == 0 ? stat_buf.st_size : -1;
        //  user u;
        string peer_port = to_string(myowninfo.port);
        string peer_id = to_string(myowninfo.userid);
        int no_of_chunks;
        //  cout<<"The file size is "<<rc<<endl;
        no_of_chunks = ceil(sizeInBytes / chunksize);
        // cout << "The no.of chunks is " << no_of_chunks << endl;
        //  for(int i=0;i<no_of_chunks;i++){

        //     //  peers.push_back(u);
        //     which_chunk_which_peer[file_name][i].push_back(u);
        //  }

        file_size = to_string(sizeInBytes);
         string sha1= calculatesha(file_path);
        string d = "upload_file";
        string to_tracker = d + " " + gr_id + " " + file_name + " " + file_size + " " + peer_port + " " + peer_id + " " + file_path + " " + to_string(no_of_chunks)+" "+sha1;

        f.file_name = file_name;
        f.file_size = sizeInBytes;
        f.groupid = stoi(gr_id);
        f.no_of_chunks = no_of_chunks;
        // cout << "time to display";
        int i = 0;
        // cout<<"hi";
        // while(1)
        //     {
        //         /* First read file in chunks of 256 bytes */

        //         // pthread_t t;

        //         // pthread_create(&t,NULL, senddata,(void* )connfd);
        //     FILE *fp = fopen("hi.txt","rb");
        //     if(fp==NULL)
        //     {
        //         printf("File opern error");
        //         // return 1;
        //     }
        //          char buff[chunksize];
        //         int nread = fread(buff,1,chunksize,fp);
        //         printf("Bytes read %d \n", nread);

        //         /* If read was success, send data. */
        //         // if(nread > 0)
        //         // {

        //         //     int a_size = sizeof(buff) / sizeof(char);
        //         //     string s_a = convertToString(buff, a_size);
        //         //     // printf("Sending \n");
        //         //     // char num[128];
        //         //     // strcpy(num,to_string(nread).c_str());
        //         //     // send(connfd,num,sizeof(num),0);
        //         //     // send(connfd, buff, sizeof(buff),0);

        //         //     f.chunk_and_data[i]=s_a;
        //         //     i++;

        //         // }

        //         // /*
        //         //  * There is something tricky going on with read ..
        //         //  * Either there was error, or we reached end of file.
        //         //  */
        //         if (nread < chunksize)
        //         {
        //             if (feof(fp))
        //                 printf("End of file\n");

        //             if (ferror(fp))
        //                 printf("Error reading\n");
        //             break;
        //         }

        //     }
    
        std::ifstream fin(file_path,ios::binary|ios::in);
     
  
    size_t tchunks = ceil(stoi(file_size)*1.0 / (512*1024));
    size_t last_ch_s = stoi(file_size) % (512*1024);
    // cout<<"the tchunks are: "<<tchunks<<endl;
    // cout<<"the last  are: "<<last_ch_s;
    for (size_t i = 0; i < tchunks; i++)
    {
        size_t this_chunk_size = i == tchunks - 1 ? last_ch_s : 512*1024; 

        vector<char> chunk_data(this_chunk_size);
        fin.read(&chunk_data[0],this_chunk_size); 
        string s(chunk_data.begin(),chunk_data.end());
    
        f.chunk_and_data[i]=s;

        cout<<"Uploading chunk of size "<<s.length()<<endl;

     

    }



        // char buffer[512 * 1024];
        // std::ifstream fin(file_path, ios::binary);

        // for (int i = 0; i < no_of_chunks; i++)
        // {
        //     bzero(buffer, sizeof(buffer));
        //     fin.read(buffer, sizeof(buffer));

        //     size_t count = fin.gcount();
        //     // cout << "No. of bytes read = " << count << endl;
        //     if (!count)
        //         break;

        //     string s(buffer);
        //     f.chunk_and_data[i]=s;
        //     // newfile->chunks[i] = s;
        // }

        // std::ifstream fin(file_path, std::ifstream::binary);
        // std::vector<char> basket(chunksize, 0); //reads only the first 1024 bytes

        // while (!fin.eof())
        // {
        //     fin.read(basket.data(), basket.size());
        //     std::streamsize s = fin.gcount();
        //     // cout<<buffer.data()<<endl;
        //     // cout<<"=============================================================="<<endl;
        //     f.chunk_and_data[i] = basket.data();
        //     i++;

        //     ///do with buffer
        // }

        fileinformation[file_path] = f;
        // for (auto it = f.chunk_and_data.begin(); it != f.chunk_and_data.end(); it++)
        // {
        //     cout << it->first << "->" << it->second << endl;
        // }

        // fstream fin("hi.txt");
        // cout << "fstream succesful" << endl;
        // cout << "jaso" << endl;
        vector<char> store;
        // cout << "The tracker string is: " << to_tracker << endl;
        char *c = new char[to_tracker.length() + 1];
        strcpy(c, to_tracker.c_str());
        write(socket_desc, c, strlen(c));
        // bzero(c,256);
        char ch[256];
        bzero(ch, 256);
        read(socket_desc, ch, 256);
        printf("the message from server is: %s ", ch);
        cout<<endl;
        cout<<endl;
    }

    else if (words[0] == "download_file")
    {

        // cout << "In the download  command";
        string d = "download_file";
        string gr_id = words[1];
        string file_name;
        string file_path = words[2];
         
        string destination_path = words[3];
        char cha = '/';
        string cur_dir_path;
        int count = std::count(file_path.begin(), file_path.end(), cha);
        if (count == 0)
        {
            file_name = file_path;
            char cwd[256];
            if (getcwd(cwd, sizeof(cwd)) == NULL)
                perror("getcwd() error");
            else
                cur_dir_path = cwd;
            file_path = cur_dir_path + "/" + file_name;
        //     cout << "the cwd is : " << cur_dir_path << endl;
        }
        else
        {
            int index;
            for (int i = 0; i <= file_path.length(); i++)
            {
                if (file_path[i] == cha)
                {
                    index = i;
                }
            }
            file_name = file_path.substr(index + 1);
        }
        download_details dikki;
        dikki.status="Downloading";
        dikki.grid=stoi(gr_id);
        downloads_dikhao[file_path]=dikki;
        //  for (auto it=downloads_dikhao.begin();it!=downloads_dikhao.end();it++)
        // {

        //     // cout << "File :  " << y << endl;

        //     if(it->second.status=="Downloading"){
        //         cout<<"[D] "<<"["<<it->second.grid<<"] "<<pathtoname(it->first)<<endl;
        //     }
        //     else{
        //         cout<<"[C] "<<"["<<it->second.grid<<"] "<<pathtoname(it->first)<<endl;
        //     }
        // }
        string file_size;
        ifstream ifs(file_name);
        ifs.seekg(0, ios::end);
        unsigned long long int sizeInBytes = ifs.tellg();
        ifs.close();
        file_size = to_string(sizeInBytes);
        string peer_id = to_string(myowninfo.userid);
        string to_tracker = d + " " + gr_id + " " + file_name + " " + file_size + " " + file_path + " " + peer_id + " " + destination_path;
        // pthread_join(run_as_server,NULL);
        // cout << "The totracker string is: " << to_tracker << endl;
        char *c = new char[to_tracker.length() + 1];
        strcpy(c, to_tracker.c_str());
        write(socket_desc, c, strlen(c));
        // bzero(c,256);
        char ch[256];
        bzero(ch, 256);
        read(socket_desc, ch, 256);
        printf("the message from server is: %s ", ch);
        // cout<<"THe ch is: "<<ch;
        vector<string> word;
        space_separated_words(ch,word);
        string chunkn = word[1];
        string sha1=word[2];
        // chunkn = chunkn.substr(1);
        // cout << chunkn << endl;
        // sleep(30);
        //=======================================okay till here checked=============================

        int no_of_chunks_for_the_current_file = stoi(chunkn);

        int random_chunk = rand() % no_of_chunks_for_the_current_file;
        fout.open(destination_path, ios::out | ios::trunc);

        for (int i = 0; i < no_of_chunks_for_the_current_file ; i++)
        {

            // int chunk_no = i % no_of_chunks_for_the_current_file;
            // cout << "Entering the loop for " << i + 1 << " time  and the chiunk no is: " << chunk_no << endl;
            string to_tracker = file_path + " " + to_string(i);
            // pthread_join(run_as_server,NULL);

            char *c1 = new char[to_tracker.length() + 1];
            strcpy(c1, to_tracker.c_str());
            write(socket_desc, c1, strlen(c));
            // bzero(c,256);
            char ch1[256];
            bzero(ch1, 256);
            read(socket_desc, ch1, 256);
            printf("the message from server is: %s \n", ch1);
            // string m=convertToString(ch1,strlen(ch1));
            // ch1.append(m);
            string m = ch1;
            string final = m + " " + destination_path + " " + to_string(no_of_chunks_for_the_current_file);
            // cout << final << endl;
            //===================================================iske upar tak check karo=================================
            pthread_t eachchunk;

            pthread_create(&eachchunk, NULL, ask_chunk, (void *) final.c_str());

            string inform = file_path + " " + to_string(i) + " " + to_string(myowninfo.userid);

            char *c2 = new char[inform.length() + 1];
            strcpy(c2, inform.c_str());
            write(socket_desc, c2, strlen(c2));
            pthread_join(eachchunk, NULL);

            //==========================================check till here===============================

            
        }
         fout.close();
        string checksha=calculatesha(file_path);
        if(checksha==sha1){
            cout<<"The file is downloaded correctly as the sha1 hashes match";
            cout<<endl;
        cout<<endl;
        }
           downloads_dikhao[file_path].status="Complete";
        //    downloads_dikhao[file_path].status="Complete";
        // dikki.status="Completed";
        // dikki.grid=stoi(gr_id);
        // downloads_dikhao[file_path]=dikki;
        //     for (auto it=downloads_dikhao.begin();it!=downloads_dikhao.end();it++)
        // {

        //     // cout << "File :  " << y << endl;

        //     if(it->second.status=="Downloading"){
        //         cout<<"[D] "<<"["<<it->second.grid<<"] "<<(it->first)<<endl;
        //     }
        //     else{
        //         cout<<"[C] "<<"["<<it->second.grid<<"] "<<(it->first)<<endl;
        //     }
        // }
        //=============================================== code till here             ====================================
        // for (auto y : token)
        // {

        //     cout << "Group " << y << endl;
        // }
        // }
        // else{
        //      printf("the message from server is: %s ",ch);
        // }
    }


    else if (words[0] == "stop_sharing")
    {
        
        

        int gr_id = stoi(words[1]);
         string file_path = (words[2]);
        // string d = "list_files";
        // string to_tracker = d + " " + to_string(gr_id) + " " + to_string(myowninfo.userid);
        char *c = new char[buffer.length() + 1];
        strcpy(c, buffer.c_str());
        write(socket_desc, c, strlen(c));
        // bzero(c,256);
        char ch[256];
        bzero(ch, 256);
        read(socket_desc, ch, 256);
        printf("the message from server is: %s ",ch);
        cout<<endl;
        cout<<endl;
    }

    else if (words[0] == "show_downloads\n")
    {
        
        printf("The output for show downloads is:   :\n");
        for (auto it=downloads_dikhao.begin();it!=downloads_dikhao.end();it++)
        {

            // cout << "File :  " << y << endl;

            if(it->second.status=="Downloading"){
                cout<<"[D] "<<"["<<it->second.grid<<"] "<<pathtoname(it->first)<<endl;
            }
            else{
                cout<<"[C] "<<"["<<it->second.grid<<"] "<<pathtoname(it->first)<<endl;
            }
        }
        // }
        // else{
        //      printf("the message from server is: %s ",ch);
        // }
        cout<<endl;
        cout<<endl;
    }
    else{
        colorred("INVALID COMMAND......");
        cout<<endl;
        cout<<endl;
    }
}
int main(int argc, char *argv[])
{

    cout<<endl;
    coloryellow("............WELCOME TO THE PEER TO PEER FILE SHARING SYSTEM.........");
    cout<<endl;
    cout<<endl;
    coloryellow("............YOU ARE VIEWING THE PEER SIDE TERMINAL.........");
    cout<<endl;
     cout<<endl;
    int socket_desc, n, portno;
    struct sockaddr_in server;
    char *message, server_reply[2000];
    char buffer[256];
    //Create socket
    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no info provided\n");
        exit(1);
    }
    char *ip_port = argv[1];
    string d(ip_port, ip_port + strlen(ip_port));
    // cout<<"hello";

    vector<string> res = split(d, ':');
    // vector<string> res;
    // res.push_back("hey");
    //  res.push_back("hey");
    fstream newfile;
    newfile.open(argv[2], ios::in);

    if (newfile.is_open())
    {
        string tp;

        getline(newfile, tp);
        // cout<<tp;
        portno = atoi(tp.c_str());
        getline(newfile, tp);
        //  server.sin_addr.s_addr =inet_addr(tp.c_str());
        newfile.close(); //close the file object.
    }

    pthread_t run_as_server;
    pthread_create(&run_as_server, NULL, client_as_server, (void *)ip_port);
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }

    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    // server.sin_port = htons(8888);
     server.sin_port = portno;
    //Connect to remote server
    if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        puts("connect error");
        return 1;
    }

    puts("Connected with the tracker.......");
    cout<<endl;
    while (true)
    {

        cout<<endl;
        colorgreen("oooooooooooooooooooooooooooooooooooooooooooooooooooo");
        cout<<endl;
         cout<<endl;
        coloryellow("Please enter the commmand: ");

        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        // cout<<endl;
        vector<string> words;
        // puts(buffer);
        space_separated_words(buffer, words);
        // for(auto x:words){
        //     cout<<x<<endl;
        // }
        // cout<<"hi checking th ewords";
        // for(auto x:words){

        //     cout<<x<<" ";
        // }
        // cout<<endl;
        if (words[0] == "create_user")
        {
            words.push_back(res[1]);
        }
        if (words[0] == "download_file\n")
        {
            // words.push_back(res[1]);
            // pthread_join(run_as_server,NULL);
        }
        checkcommand(socket_desc, words, buffer, run_as_server);
        // n = write(socket_desc, buffer, strlen(buffer));
        // if (n < 0)
        //     puts("ERROR writing to socket");
        // bzero(buffer, 256);
        // n = read(socket_desc, buffer, 255);
        // if (n < 0)
        //     puts("ERROR reading from socket");
        // printf("%s\n", buffer);
        // close(socket_desc);
    }
    return 0;
}

void *client_as_server(void *ip_port)
{

    int sockfd, newsockfd, portno, pid;
    int *new_sock;
    char *message;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char *mess = (char *)ip_port;
    string str(mess, mess + strlen(mess));
    // cout<<endl<<str;

    vector<string> res = split(str, ':');
    // for(auto l:res){
    //     cout<<"inres"<<l<<endl;
    // }
    // cout << "in the clientas server program" << endl;
    // cout<<endl;
    // cout<<mess;
    // vector<string> res;
    // res.push_back("hey");
    // res.push_back("hey");
    portno = stoi(res[1]);

    // if (argc < 2)
    // {
    //     fprintf(stderr, "ERROR, no info provided\n");
    //     exit(1);
    // }

    // fstream newfile;
    // newfile.open(argv[1], ios::in);

    // if (newfile.is_open())
    // {
    //     string tp;

    //     getline(newfile, tp);
    //     // cout << tp;
    //     portno = atoi(tp.c_str());

    //     newfile.close();
    // }

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

    //accepting connections in an infinite loop
    while (1)
    {

        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        // cout << "the accept has been done";
        if (newsockfd < 0)
        {
            error("ERROR on accept");
            return (void *)1;
        }

        pthread_t sniffer_thread;
        new_sock = (int *)malloc(1);
        *new_sock = newsockfd;

        if (pthread_create(&sniffer_thread, NULL, communication_with_peer, (void *)new_sock) < 0)
        {
            perror("could not create thread");
            return (void *)1;
        }

        // pthread_join(sniffer_thread, NULL);

        // close(newsockfd);
    } /* end of while */
    close(sockfd);
    return 0;
}

void *communication_with_peer(void *new_sock)
{
    // cout << "in the communiacation with peer program" << endl;
    int sock = *(int *)new_sock;
    int n;
    char *message, client_message[2000];
    char buffer[256];
    // int userid_of_running_client;
    // send(sock, "Hello, world!\n", 13, 0);
    // while (true)
    // {
    //     bzero(buffer, 256);

    //     n = read(sock, buffer, 255);
    //     if (n < 0)
    //         error("ERROR reading from socket");
    //     printf("Message from the client: %s", buffer);
    //     vector<string> words;
    //     space_separated_words(buffer, words);
    //     // cout << "Fromo client: " << buff << endl;
    //     if (words[0] == "create_user")
    //     {
    //         userid_of_running_client = stoi(words[1]);
    //     }
    //     for(auto x:words){
    //         cout<<x<<endl;
    //     }
    //     string x = process_command(buffer, words, userid_of_running_client);
    //     char *buff1 = new char[x.length() + 1];
    //     strcpy(buff1, x.c_str());
    //     // write(socket_desc, c, strlen(c));
    //     bzero(buffer, 256);
    //     write(sock, buff1, strlen(buff1));
    // }
    // cout << endl
    //      << "ready to read";
    n = read(sock, buffer, 255);
    if (n < 0)
        error("ERROR reading from socket");
    printf("Message from the client: %s", buffer);

    vector<string> information_for_chunk;
    space_separated_words(buffer, information_for_chunk);
    int chunk_number = stoi(information_for_chunk[1]);
    string file_path_for_download = information_for_chunk[0];

    string x = fileinformation[file_path_for_download].chunk_and_data[chunk_number];

    cout<<"\ni am currently seeing string x length  "<<x.length()<<endl;

    //============================Done till here now have dinner============================================
    char *buff1 = new char[x.length() + 1];
    strcpy(buff1, x.c_str());
    // write(socket_desc, c, strlen(c));
    // cout << endl
    //      << buff1;
    string h = to_string(x.length());
    cout << "\nLLLEngth of that chunk is " << h << endl;
    char bufggg[1024];
    strcpy(bufggg,h.c_str());
    int k = send(sock, bufggg,sizeof(bufggg), 0);
    cout<<"Size of the chunk sent : "<<k<<endl;
    bzero(buffer, 256);
    // cout << "ready to write";

    const char *ptr = x.data();
    int d =write(sock,ptr,x.length());

    // int d = write(sock, buff1, strlen(buff1));
    cout << "The no.of bytes written of the chunk: " << d << endl;
    //Free the socket pointer
    // free(new_sock);
}