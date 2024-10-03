// Nahum Arnet, Christopher Caponegro
// Sources: chat gpt (only used for small questions, not to code entire thing)

#include <iostream>
#include <fstream>
#include <thread>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <iostream>
#include <filesystem>
using namespace std;

//each dictionary will handle 26 books and there is a 110 books in total. 

struct word_info {
    uint64_t count; // total number of times the word was found
    uint32_t in_books; // in how many books
    int32_t last_book; // number of last book it was found in

    // Overload the += operator
    word_info& operator+=(const word_info& other) {
        this->count += other.count;
        this->in_books += other.in_books;
        this->last_book = std::max(this->last_book, other.last_book); // Keep the latest book
        return *this;
    }
};
// "the"  last_book: -1 in_book = 1  count = 1 last_book = 1
// "the"  book: 2  last_book = 1, count = 2 last_book = 2
class Dict {
private:
    unordered_map<string, word_info> dict;
public:
    Dict() {}
    void add_word(const string& word, int book) {
        if (dict.find(word) == dict.end()) { // Check if word is not in our map
            dict[word] = { 1, 1, book }; // the count is 1 in 1 book and book == last_book
        }
        else {
            dict[word].count++; // increase the word count 
            if (book > dict[word].last_book) // and if the book you are in > last word in dictionaty book and the specifed word then 
                dict[word].last_book = book;        } // the last book with the specified word is equal to book
    }
    unordered_map<string, word_info>& getMap() {
        return dict;
    }

    // Getter for the unordered_map
    // Non-const getter (allows modification)
    std::unordered_map<std::string, word_info>& get_dict() {
        return dict;
    }

    // Setter for the unordered_map
    void set_dict(const std::unordered_map<std::string, word_info>& new_dict) {
        dict = new_dict;
    }

    void print_map(){

        std::vector<std::pair<std::string, word_info>> sorted_entries(dict.begin(), dict.end());

        // Sort the vector by word count in descending order
        std::sort(sorted_entries.begin(), sorted_entries.end(),
                  [](const std::pair<std::string, word_info>& a, const std::pair<std::string, word_info>& b) {
                      return a.second.count > b.second.count; 
                  });

        // Print the top 300 words
        int limit = std::min(300, static_cast<int>(sorted_entries.size()));
        for (int i = 0; i < limit; ++i) {
            const auto& entry = sorted_entries[i];
            const std::string& word = entry.first;
            const word_info& info = entry.second;

            
            std::cout << "Word: " << word << "\n" << "Count: " << info.count << "\n";
                     // << "In Books: " << info.in_books << "\n"
                     // << "Last Book: " << info.last_book << "\n";

            cout << endl;
        }
    }
    void trim_map(){
        // Trim all words that have count of 1 and print out total number of words
        auto it = dict.begin();  // Get an iterator to the beginning of the map
        while (it != dict.end()) {
            if (it->second.count == 1) {
                it = dict.erase(it); 
            } else {
                ++it; 
            }
        }
    
        std::cout << "Total number of words is: " << dict.size() << std::endl;
    }
    
    
};


namespace fs = std::filesystem;

// open a single book
void openfile(const fs::path& path, int book_num, Dict &d) {

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return;
    } 

    // for each word in the file, lower case it and add it to the dictionary
    string word;
    while (file >> word) {
        // hyph-enated
        // Chrises'
        //  132nd 
        transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });

        d.add_word(word, book_num);
    }
}


 void openFileMultithreaded(Dict &d, uint32_t a, uint32_t b, vector<fs::directory_entry> &files) {
    
    // Iterate over files from index a to b (b exclusive)
    for (int i = a; i < b && i < files.size(); ++i) {
        // std::cout << files[i].path() << std::endl;  // Print file paths
        openfile(files[i].path(), i, d);    
                                              
    }
   
}


void mergeMaps( Dict &d1, Dict &d2, Dict &d3, Dict &d4) {
    //merge 1 and 2 into 2
    for (auto it = d1.get_dict().begin(); it != d1.get_dict().end(); it++) {
        if (d2.get_dict().find(it->first) == d2.get_dict().end()) { // if 2nd dictionary does not contain this word
            d2.get_dict()[it->first] = it->second; // add it to the dictionary
        } else {
            d2.get_dict()[it->first] += it->second;
        }
    }

    //merge 3 and 4 into 4
    for (auto it = d3.get_dict().begin(); it != d3.get_dict().end(); it++) {
        if (d4.get_dict().find(it->first) == d4.get_dict().end()) { // if 2nd dictionary does not contain this word
            d4.get_dict()[it->first] = it->second; // add it to the dictionary
        } else {
            d4.get_dict()[it->first] += it->second;
        }
    }

    //merge 4 and 2 into 2
    for (auto it = d4.get_dict().begin(); it != d4.get_dict().end(); it++) {
        if (d2.get_dict().find(it->first) == d2.get_dict().end()) { // if 2nd dictionary does not contain this word
            d2.get_dict()[it->first] = it->second; // add it to the dictionary
        } else {
            d2.get_dict()[it->first] += it->second;
        }
    }

    //2 now contains the complete dictionary

}


//Arguement inputs:  ./executbale (path to books)
int main(int argc, char* argv[]) {
    Dict d1;
    Dict d2;
    Dict d3;
    Dict d4;

    string path = argv[1];

    int book_num = 0;
    
    // Store files in a vector
    vector<fs::directory_entry> files;

    // Collect all files in the directory
    try{
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {  // Ensure it's a file (not a directory) and its a .txt
                files.push_back(entry);
            }
        }
    } catch (const fs::filesystem_error& err) {
        std::cerr << "Filesystem error: " << err.what() << std::endl;
    }



    // four threads are utilized to split load over files
    thread t1(openFileMultithreaded, std::ref(d1), 0,  28, ref(files));
    thread t2(openFileMultithreaded, std::ref(d2), 28, 56, ref(files));
    thread t3(openFileMultithreaded, std::ref(d3), 56, 83, ref(files));
    thread t4(openFileMultithreaded, std::ref(d4), 83, 110, ref(files));

    // joining threads
    t1.join(); 
    t2.join();
    t3.join();
    t4.join();


    mergeMaps(d1, d2, d3, d4); 

    //d2 contains complete dictionary so trim words with count == 1 and print top 300 words (:
    d2.trim_map();
    d2.print_map();

    //d1.print_map();
    //d2.print_map();
    //d3.print_map();
    //d4.print_map();

   

}

