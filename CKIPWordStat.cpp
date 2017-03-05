#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "Usage : CKIPWordStat <CKIP_OUTPUT_FILE>" << endl;
        exit(1);
    }

    //Pre-processing
    ifstream read;
    ofstream write;
    char checkChar;
    char checkChar_Temp;

    vector<string> WordStack;
    vector<string> CategoryStack;
    string checkString;

    /** Preprocessing to Mid-Transition: Convert '-95' '60' bit to CRLF**/
    read.open(((string)argv[1]).c_str());
    write.open(((string)argv[1] + ".mid").c_str());
    if(read.is_open())
    {
        while(!read.eof())
        {
            read.get(checkChar);
            // (-95 AND 64 Are the character "Full Space to Separate each word by CKIP")
            if ((int)checkChar == -95)
            {
                checkChar_Temp = checkChar;
                read.get(checkChar);
                if ((int)checkChar == 64)
                {
                    write << '\n';
                    continue;
                }
                else
                {
                    write.put(checkChar_Temp);
                    write.put(checkChar);
                    continue;
                }
            }
            else
            {
                write.put(checkChar);
            }
        }
    }
    else
    {
        cout << "File Read Error!" << endl;
        exit(1);
    }
    write.flush();
    write.close();
    read.close();
    cout << "Pre-processing is finished." << endl;

    /**Mid-Transition to Post-processing: Remove Duplicated CRLF**/
    read.open(((string)argv[1] + ".mid").c_str());
    write.open(((string)argv[1] + ".post").c_str());
    if(read.is_open())
    {
        while(!read.eof())
        {
            read.get(checkChar);
            if ((int)checkChar == 10)
            {
                while((int)checkChar == 10)
                {
                    read.get(checkChar);
                    if (read.eof())
                    {
                        checkChar = ' ';
                    }
                }
                write << '\n';
                write.put(checkChar);
                continue;
            }
            else
            {
                write.put(checkChar);
                continue;
            }
        }
    }
    else
    {
        cout << "File Read Error!" << endl;
        exit(1);
    }
    write.flush();
    read.close();
    write.close();

    /**Word Counting Process**/
    read.open(((string)argv[1] + ".post").c_str());
    outer:
    while(!read.eof())
    {
        getline(read, checkString);
        if (WordStack.size() == 0)
        {
            WordStack.push_back(checkString);
            continue;
        }
        for(int i = 0; i < WordStack.size(); i++)
        {
            if (checkString.compare(WordStack[i]) == 0)
            {
                goto outer;
            }
        }
        WordStack.push_back(checkString);

    }

    /**The last element of the vector seems to be a pointer, discard it.**/
    int AllDistinctWordCount = WordStack.size() - 1;

    string** WordStat = new string*[AllDistinctWordCount];
    for (int i = 0; i < AllDistinctWordCount; i++)
    {
        WordStat[i] = new string[3];
        (WordStat[i][0]) = (WordStack[i]);
    }
    read.close();

    /**Counting Words**/
    read.open(((string)argv[1] + ".post").c_str());
    while(!read.eof())
    {
        getline(read, checkString);
        for (int i = 0; i < AllDistinctWordCount; i++)
        {
            if (checkString.compare((WordStat[i][0])) == 0)
            {
                (WordStat[i][2]).append("*");
            }
        }
    }
    read.close();


    /**Separating the Word Category**/
    for (int i = 0; i < AllDistinctWordCount; i++)
    {
        /** This will cause some serious issues, such as double ( mark **/
        /** Resolve the issue of double ( by using rfind **/
        (WordStat[i][1]).append((WordStat[i][0]).substr((int)(WordStat[i][0]).rfind("("), (int)(WordStat[i][0]).length()));
        (WordStat[i][0]).erase((int)(WordStat[i][0]).rfind("("), (int)(WordStat[i][0]).length());
    }

    /**Display WordCount Result**/
    for (int i = 0; i < AllDistinctWordCount; i++)
    {
        cout << WordStat[i][0] << "\t\t" << WordStat[i][1] << "\t\t" << (WordStat[i][2]).length() << endl;
    }


    /**Counting Distinct Category**/
    for (int i = 0; i < AllDistinctWordCount; i++)
    {
        bool hasSameWord = false;
        if (CategoryStack.size() == 0)
        {
            CategoryStack.push_back((WordStat[i][1]));
            continue;
        }
        for (int j = 0; j < CategoryStack.size(); j++)
        {
            if ((WordStat[i][1]).compare(CategoryStack[j]) == 0)
            {
                hasSameWord = true;
                break;
            }
        }
        if (hasSameWord == true)
        {
            continue;
        }
        else
        {
            CategoryStack.push_back(WordStat[i][1]);
        }
    }

    /** Summing Category Counts**/

    int AllDistinctCategoryCount = CategoryStack.size(); // -1
    string** CategoryStat = new string*[AllDistinctCategoryCount];
    for (int i = 0 ; i < AllDistinctCategoryCount; i++)
    {
        CategoryStat[i] = new string[2];
        CategoryStat[i][0] = CategoryStack[i];
    }

    for (int i = 0; i < AllDistinctWordCount; i++)
    {
        for (int j = 0; j < AllDistinctCategoryCount; j++)
        {
            if ((WordStat[i][1]).compare((CategoryStat[j][0])) == 0)
            {
                (CategoryStat[j][1]).append("*");
                break;
            }
        }
    }

    for (int i = 0; i < AllDistinctCategoryCount; i++)
    {
        cout << CategoryStat[i][0] << "\t\t" << (CategoryStat[i][1]).length() << endl;
    }

    ofstream ckipWordStatOutput;
    ofstream ckipCategoryStatOutput;
    ckipWordStatOutput.open(((string)argv[1] + "_WordStat.csv").c_str());
    ckipCategoryStatOutput.open(((string)argv[1] + "_CategoryStat.csv").c_str());

    for (int i = 0 ; i < AllDistinctWordCount; i++)
    {
        ckipWordStatOutput << WordStat[i][0] << "," << WordStat[i][1] << "," << (WordStat[i][2]).length() << endl;
    }

    for (int i = 0 ; i < AllDistinctCategoryCount; i++)
    {
        ckipCategoryStatOutput << CategoryStat[i][0] << "," << (CategoryStat[i][1]).length() << endl;
    }

    ckipWordStatOutput.flush();
    ckipCategoryStatOutput.flush();
    ckipWordStatOutput.close();
    ckipCategoryStatOutput.close();

    return EXIT_SUCCESS;

}
