#include <iostream>
#include <cmath>
#include <bits/stdc++.h>

#define B 2 // number of records in a disk block
#define M 1 // max. no. of blocks that can be present in main memory

using namespace std;

string generateRandomStrings()
{
    string res = "";
    for (int i = 0; i < 3; i++)
    {
        char ch = 'a' + rand() % 26;
        res += ch;
    }
    return res;
}

void create_disk_blocks(int n)
{
    ifstream ReadFile("dataset.txt");
    for (int i = 1; i <= n; i++)
    {
        ofstream WriteFile(to_string(i) + ".txt");
        for (int j = 1; j <= B; j++)
        {
            // if (ReadFile.eof())
            // {
            //     break;
            // }
            string buffer;
            getline(ReadFile, buffer);
            if (buffer.empty())
            {
                WriteFile << "NULL"
                          << "\n";
                break;
            }
            else
            {
                WriteFile << buffer << "\n";
            }
        }
        if (i != n)
        {
            WriteFile << to_string(i + 1) + ".txt"
                      << "\n";
        }

        WriteFile.close();
    }
    ReadFile.close();
}

struct data
{
    string t_id;
    int amount;
    string name;
    string category;
};

bool compareAmount(struct data i , struct data j){
    return (i.amount < j.amount);
}

struct compareAmount_st {
    bool operator()(struct data i , struct data j)
    {
        // return "true" if "p1" is ordered
        // before "p2", for example:
        return (i.amount > j.amount);
    }
};

int get_lines(string file) {
    int numLines = 0;
    ifstream in(file);
    std::string unused;
    while ( std::getline(in, unused) )
        ++numLines;

    return numLines;
}

void split_file(string file)
{
    // splits the file into different files in which each file will contain the 'B' number of records

    ifstream file1 (file + ".txt");
    int counter=1;
    int n_records=0;
    ofstream WriteFile(file + "_" + to_string(counter) + ".txt"); // for writing the file
    while(true) {
        string buffer;
        getline(file1, buffer);
        if (buffer.empty()) {
            WriteFile << "NULL\n";
            WriteFile.close();
            break;
        }
        if(n_records==B) {
            // close the file and create a new file
            counter++;
            string next_file = file + "_" + to_string(counter) + ".txt";
            WriteFile << next_file << "\n";
            WriteFile.close();
            WriteFile.open(next_file);
            n_records=0;
        }
        n_records++;
        WriteFile << buffer << "\n";
    }

}

void N_Way_Merge(vector<struct data> & vec, int start_run, int runs, string input_file="run", string output_file="result"){
    // What is the use of vec?
    // taken two files
    // input: file name of the run file like "run{i}.txt";
    // output: expected output file name (this is used in case 2)

    ofstream WriteFile(output_file+".txt");

    int N = min(M-1, runs); // N runs merge at a time
    // M*B is the number of rows that can be in the queue
    int queue_limit = M * B;
    // trick is to keep pushing till the queue does not get full
    // Note: if the queue gets full pop only B number of rows (imp)

    priority_queue<struct data, vector<struct data>, compareAmount_st> pq;
//    cout << "Entries:" << endl;

    int counter=0;  // for skipping the entries

    int n_rows = get_lines(input_file + "1.txt");  // try to derive formula?

    for(int l=1; l<=n_rows/B; l++) {
        for (int i = start_run; i <= runs; i++)
        {
            // this iteration is for reading all the files
            ifstream file1 (input_file + to_string(i) + ".txt");
            int skip = counter*B;
            for(int i=0; i<skip; i++) {
                // for skipping entries
                string buffer;
                getline(file1, buffer);
                // TODO: check the buffer
                if (buffer.empty()) {
                    break;
                }
            }

            if(pq.size() == queue_limit) {
                // this means the queue is full and we need to pop B no of rows
                for(int k=1; k<=B; k++) {
                    struct data obj = pq.top();
                    pq.pop();
//                    cout << obj.amount << "\n";
                    WriteFile << obj.t_id << "," << obj.amount << "," << obj.name << "," << obj.category << "\n";
                }
            }

            for (int i = 1; i <= B; i++)
            {
                string buffer;
                getline(file1, buffer);
                // TODO: check the buffer
                if (buffer.empty()) {
                    break;
                }
                int id, amount, cat;
                char name[20];
                sscanf((char *)&buffer[0], "%d,%d,%[^,],%d", &id, &amount, name, &cat);
                // cout<< buffer << endl;
                // printf("%d,%d,%s,%d\n", id, amount, name, cat);
                struct data obj;
                obj.t_id = to_string(id);
                obj.amount = amount;
                obj.name = name;
                obj.category = to_string(cat);

                pq.push(obj);
            }
        }
        counter++;
    }

    // Now empty the queue
    while (!pq.empty()) {
        struct data obj = pq.top();
        pq.pop();
//        cout << obj.amount << "\n";
        WriteFile << obj.t_id << "," << obj.amount << "," << obj.name << "," << obj.category << "\n";
    }
    WriteFile.close();
    split_file(output_file);
}

void N_Way_Merge2(vector<struct data> & vec, int runs, string input_file="run") {
    // vec: no use remove it !
    // runs: total runs file created
    // for this case it will be M <= runs; which means we have to create multiple passes

    // trick: In general, we can merge Min(#runs, M-1) runs at a time.

    int n_merges = min(M - 1, runs); // number of merges
    int n_runs = n_merges;
    int start_run=1;

    int counter = 1;    // for intermediate files
    // can do better condition ???
    bool flag=true;
    do {
        n_runs= min(n_runs, runs);   // To know how much files to merge
        if(n_runs==runs) {
            // end condition
            flag=false;
        }
        string output_file = input_file + "_mp" + to_string(counter); // intermediate files
        cout << "------------------------------------\n";
        printf("%d, %d, %s, %s\n", start_run, n_runs, &input_file[0], &output_file[0]);
        N_Way_Merge(vec, start_run, n_runs, input_file, output_file);
        cout << "------------------------------------\n";
        start_run=n_runs+1; // next file to start
        n_runs = n_runs+n_merges; // max files that can be merged
        counter++;
    } while(flag);

    // (counter-1) is the number of intermediate files or we can say number of runs
    // the process will be repeated
    int intermediate_runs = counter-1;
    if ( M > intermediate_runs) {
        cout << "Int Files:" << intermediate_runs << endl;
        N_Way_Merge(vec, 1, intermediate_runs, input_file=input_file+"_mp");
    }
    else {
        // still more passes to be made
        cout << "Int Files:" << intermediate_runs << endl;
        N_Way_Merge2(vec, intermediate_runs, input_file=input_file+"_mp");
    }

}


int main()
{
    int records;
    cout << "Enter the number of records in dataset: ";
    cin >> records;
    if(M==1){
        cout<<"Cannot merge"<<endl;
        return 0;
    }
    srand(time(0));
    ofstream newFile("dataset.txt");
    for (int i = 1; i <= records; i++)
    {
        newFile << i << ","
                << 1 + rand() % (60000 - 1 + 1) << "," // generating random no. between 1 and 60000
                << generateRandomStrings() << ","
                << 1 + rand() % (1500 - 1 + 1) << "\n"; // generating random no. between 1 and 1500
    }

    newFile.close();

    int num_of_disk_blocks = ceil(records / float(B));
    cout << num_of_disk_blocks << " disk blocks of size " << B << " will be created." << endl;
    // disk blocks represents files like 1.txt, 2.txt, 3.txt etc

    create_disk_blocks(num_of_disk_blocks);

    int num_of_runs = ceil(num_of_disk_blocks / float(M));
    cout << "Number of runs will be " << num_of_runs << endl;

    // for string each row
    vector<struct data> testVector;

    // creating sorted runs
    for (int i = 1; i <= num_of_runs; i++)
    {
        static int prev = 1;
        int next;
        next = i * M;
        while (prev <= next)
        {
            string file_name = to_string(prev) + ".txt";
            ifstream myFile(file_name);
            if (myFile)
            {
                // reading B  rows of disk blocks
                for (int j = 1; j <= B; j++)
                {
                    // if (myFile.eof())
                    // {
                    //     break;
                    // }
                    string buffer;
                    getline(myFile, buffer);
                    if (buffer.empty())
                    {
                        break;
                    }
                    stringstream oneRow(buffer);
                    vector<string> storeRow;

                    // Reading single row of each disk blocks
                    while (oneRow.good())
                    {

                        string substr;
                        getline(oneRow, substr, ',');
                        storeRow.push_back(substr);
                    }

                    // cout << "size (" << storeRow.size() << ") :";
                    for (int i = 0; i < storeRow.size(); i += 4)
                    {

                        if (storeRow[i] == "NULL")
                        {
                            break;
                        }
                        else
                        {
                            // cout << storeRow[i] << " " << storeRow[i + 1] << " " << storeRow[i + 2] << " " << storeRow[i + 3] << endl;
                            struct data obj;
                            obj.t_id = storeRow[i];
                            obj.amount = stoi(storeRow[i + 1]);
                            obj.name = storeRow[i + 2];
                            obj.category = storeRow[i + 3];
                            testVector.push_back(obj);
                        }
                    }
                }
                myFile.close();
            }
            else
            {
                break;
            }

            prev++;
        }
        cout<<"\n";

        cout<< "Run " << i << ":"<<endl;
        //sorting Runs
        sort(testVector.begin(), testVector.end(),compareAmount);
        ofstream runs("run" + to_string(i) + ".txt");
        for(auto x: testVector){
//            cout << x.t_id <<" "<<x.amount <<" "<<x.name<<" "<<x.category <<endl;
            runs << x.t_id <<","<<x.amount <<","<<x.name<<","<<x.category <<endl;
        }
        runs.close();
        split_file("run" + to_string(i));
        testVector.clear();
        cout<< "\n";
    }

    // N way merge
    if(M > num_of_runs){
        // for this case the files will be sorted in a single run
        N_Way_Merge(testVector, 1, num_of_runs);
    }
    else {
        // for this case merge passes will be created
        N_Way_Merge2(testVector, num_of_runs);
    }

    return 0;
}
