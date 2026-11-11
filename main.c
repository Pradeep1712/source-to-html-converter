/* SOURCE TO HTML CONVERTER
Name : Goutham C P
Batch ID : 26001_059

OVERVIEW :
->This project implements a Source‑to‑HTML Converter with Lexical Analysis in C.
->The program takes a source file (.c) as input, which is read by main.c. The contents of this source file are then
 written into an HTML file (converted.html) for display in a browser.
->It reads a C source file line by line, tokenizes the content, classifies tokens (keywords, identifiers, operators, literals,
special characters),spaces  and then generates an HTML file that visually highlights these categories with colors and styles.
It also validates file arguments and checks for balanced brackets.

FEATURES :

Command Line User Interface : ./a.out filename(.c)
eg :  ./a.out sample.c >textfilename.txt

filename - File where source code(.c) is present
textfilename  - File where you can see complete tokenization and analysis before writing to html
Reason for text file instead of terminal :
 ->In terminal It is impossible to see large lines of code. that's why text file is used to view.

→ The program reads the contents of the source file and generates an HTML representation.(converted.html)
Note :  Here converted.html will be output file(defined in code)

. File Validation
->Ensures the input filename ends with .c.
->Ensures the filename contains only one dot
->Reports errors for invalid filenames.

2. File Handling
->Opens and reads source files safely.
->Reports errors if the file cannot be opened or is empty.
->Creates and writes to converted.html for output.

3. Lexical Analysis
->Detects and classifies:
->Keywords
->Identifiers
->Operators
->Special characters
->String literals
->Character constants
->Integer and floating‑point constants
->spaces
Before writing to html

4. Comment Handling
->Handle single‑line (//) and multi‑line (/* ) comments.
->Preserves comments in HTML output with green color styling.
->can Handle inline comments too.

5. Bracket Balance Checking
->Tracks counts of parentheses (), curly braces {}, and square brackets [].
->Reports errors if any are unbalanced (e.g., missing { or }).

6. Error Detection
->Reports malformed identifiers.
->Detects invalid character constants.
->Reports missing quotes or brackets.

Process :
->After generating the HTML output, open the file in a browser.
->Select all content using Ctrl+A, copy with Ctrl+C, and paste it into htmlcode.c.
->Compare the file sizes of sample.c and htmlcode.c using the ls -l command — they should be nearly identical.
->For additional verification, run identicalcheck.c to perform a character‑by‑character comparison between sample.c
 and htmlcode.c to confirm they are identical.



*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char *keywords[32] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "int", "long", "register", "return", "short", "signed", "sizeof",
    "static", "struct", "switch", "typedef", "union", "unsigned", "void",
    "volatile", "while"};

const char *operators = "+-*/%=!<>|&";
const char *specialCharacters = ":,;{}()[]\'\"";

const char *validCharConstants[11] = {
    "\\n", "\\t", "\\r", "\\0", "\\a", "\\b", "\\f", "\\v",
    "\\'", "\\\"", "\\\\"};

typedef enum successret
{
    e_success,
    e_failure
} Status;

FILE *fp;
FILE *html;
Status Read_and_Validate_Args(char *Filename);
Status OpenFile(char *Filename);
Status AnalyzeLexical();
Status AnalyseLine(char *buffer);
Status CheckCharIsOperatorOrSpecialCharacters(char ch, int num);
Status CompareWord(char *Word, const char *keywords[], const char *operators);
Status CompareSubWord(char *Word, const char *keywords[], const char *operators);
Status SetUpHTML();
Status ProcessPreprocessor(char *buffer);

int OpenBracketcount = 0;
int CloseBracketcount = 0;
int OpencurlyBracketcount = 0;
int ClosecurlyBracketcount = 0;
int LiteralOpencount = 0;
int LiteralClosecount = 0;
int opensquarebracketCount = 0;
int closesquarebracketCount = 0;
long int FileSize;
int newlineflag = 0;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Insufficient arguments\n");
        return 0;
    }
    Status Okvalid = Read_and_Validate_Args(argv[1]);
    if (Okvalid == e_failure)
    {
        printf("Error in Filename\n");
        return 0;
    }
    Status res = OpenFile(argv[1]);
    if (res == e_success)
    {
        SetUpHTML();
        AnalyzeLexical();
    }
    printf("Opencurly and close curly is %d and %d\n", OpencurlyBracketcount, ClosecurlyBracketcount);
    if (OpencurlyBracketcount != ClosecurlyBracketcount)
    {
        if (OpencurlyBracketcount > ClosecurlyBracketcount)
        {
            printf(" Error :  } missing\n");
        }
        else if (OpencurlyBracketcount < ClosecurlyBracketcount)
        {
            printf("Error : { missing\n");
        }
    }
    fclose(fp);
    fprintf(html, "</pre></body></html>");
    fclose(html);
}

Status Read_and_Validate_Args(char *Filename)
{
    int len = strlen(Filename);
    if (len < 3 || strcmp(Filename + (len - 2), ".c") != 0)
    {
        return e_failure;
    }

    // Count number of dots in the filename
    int dot_count = 0;
    for (size_t i = 0; i < len; i++)
    {
        if (Filename[i] == '.')
        {
            dot_count++;
        }
    }

    if (dot_count != 1)
    {
        return e_failure;
    }

    // All validations passed
    return e_success;
}

Status OpenFile(char *Filename)
{
    fp = fopen(Filename, "r");
    if (fp == NULL)
    {
        printf("Error opening file\n");
        return e_failure;
    }
    else if (fp != NULL)
    {
        printf("File opened successfully\n");
    }

    html = fopen("converted.html", "w");
    if (html == NULL)
    {
        printf("Error opening file\n");
        return e_failure;
    }
    else if (html != NULL)
    {
        printf("HTML File opened successfully\n");
    }
    return e_success;
}

Status SetUpHTML()
{
    fprintf(html, "<!DOCTYPE html><html><head><style>");
    fprintf(html, "body { font-family: monospace; }");
    fprintf(html, ".keyword { color: blue; font-weight: bold; }");
    fprintf(html, ".identifier { color: black; }");
    fprintf(html, ".operator { color: red; }");
    fprintf(html, ".special { color: Steel Blue ; }");
    fprintf(html, ".string { color: brown; }");
    fprintf(html, ".constant { color: purple; }");
    fprintf(html, "</style></head><body><pre>");
}

Status AnalyzeLexical()
{
    int multilineindex = 1;
    int multilineflag = 0;
    fseek(fp, 0, SEEK_END);
    FileSize = ftell(fp);
    if (FileSize == 0)
    {
        printf("content not exists in file\n");
        return e_failure;
    }
    rewind(fp);

    int ch;
    int linecount = 0;
    int i = 0;
    char *buffer = malloc(200);
    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == '\r')
        {
            continue;
        }
        i = 0;
        if (ch == EOF)
        {
            break;
        }
        buffer[0] = ch;
        ch = fgetc(fp);

        if (ch != '\n')
        {
            i += 1;

            if (ch != EOF)
            {
                buffer[i] = ch;
            }

            ch = fgetc(fp);
            i += 1;
            int skip = 0;
            int MultilineFound = 0;
            int MultilineEnd = 0;
            int SingleLineCommentend = 0;

            if (ch == '\n' || ch == EOF)
            {
                if (ch == '\n')
                {
                    buffer[i] = '\0';
                    AnalyseLine(buffer);
                    memset(buffer, '\0', 200);
                    fprintf(html, "\n");
                }
            }

            while (ch != '\n' && ch != EOF)
            {
                if (ch != EOF)
                {
                    buffer[i] = ch;
                    i += 1;
                }
                ch = fgetc(fp);

                if (ch == '\n' || ch == EOF)
                {
                    if (ftell(fp) == FileSize)
                    {
                        buffer[i] = '\0';
                        if (strlen(buffer) > 0)
                        {
                            AnalyseLine(buffer);
                            memset(buffer, '\0', 200);
                            break;
                        }
                    }
                    buffer[i] = '\0';

                    for (int j = 0; j < strlen(buffer); j++)
                    {
                        if (buffer[j] == '#')
                        {
                            if (buffer[j - 1] != '\'')
                            {
                                skip = 1;
                                ProcessPreprocessor(buffer);
                                break;
                            }
                        }
                        else if (buffer[j] == '/')
                        {
                            if (buffer[j + 1] == '/')
                            {
                                if (j == 0)
                                {
                                    buffer[strlen(buffer) - 1] = '\0';
                                    fprintf(html, "<span style='color: green;'>");
                                    for (int p = 0; p < strlen(buffer); p++)
                                    {
                                        switch (buffer[p])
                                        {
                                        case '<':
                                            fprintf(html, "&lt;");
                                            break;
                                        case '>':
                                            fprintf(html, "&gt;");
                                            break;
                                        case '\"':
                                            fprintf(html, "&quot;");
                                            break;
                                        case '&':
                                            fprintf(html, "&amp;");
                                            break;
                                        case ' ':
                                            fprintf(html, "&nbsp;");
                                            break;
                                        default:
                                            fputc(buffer[p], html);
                                            break;
                                        }
                                    }
                                    SingleLineCommentend = 1;
                                    fprintf(html, "</span>");
                                }
                                else
                                {
                                    char singlelinecommentstr[200];
                                    int begin = 0;
                                    for (int c = j; c < strlen(buffer); c++)
                                    {
                                        if (buffer[c] != '\r')
                                        {
                                            singlelinecommentstr[begin] = buffer[c];
                                            begin += 1;
                                        }
                                    }
                                    singlelinecommentstr[begin] = '\0';
                                    buffer[j] = '\0';
                                    AnalyseLine(buffer);
                                    memset(buffer, '\0', 200);
                                    fprintf(html, "<span style='color: green;'>");
                                    for (int p = 0; p < strlen(singlelinecommentstr); p++)
                                    {
                                        switch (singlelinecommentstr[p])
                                        {
                                        case '<':
                                            fprintf(html, "&lt;");
                                            break;
                                        case '>':
                                            fprintf(html, "&gt;");
                                            break;
                                        case '\"':
                                            fprintf(html, "&quot;");
                                            break;
                                        case '&':
                                            fprintf(html, "&amp;");
                                            break;
                                        case ' ':
                                            fprintf(html, "&nbsp;");
                                            break;
                                        default:
                                            fputc(singlelinecommentstr[p], html);
                                            break;
                                        }
                                    }
                                    SingleLineCommentend = 1;
                                    fprintf(html, "</span>");
                                }
                                break;
                            }
                            else if (buffer[j + 1] == '*')
                            {
                                if (j != 0)
                                {
                                    char multilinestr[200];
                                    int begin = 0;
                                    int state = 0;
                                    int recc;
                                    begin = 0;
                                    for (int c = j; c < strlen(buffer); c++)
                                    {
                                        if (buffer[c] != '\r')
                                        {
                                            multilinestr[begin] = buffer[c];
                                            begin += 1;
                                        }
                                        if (buffer[c] == '/' && buffer[c - 1] == '*' && c != strlen(buffer) - 1)
                                        {
                                            state = 1;
                                            recc = c + 1;
                                            break;
                                        }
                                    }
                                    char *buffer2;
                                    if (state == 1)
                                    {
                                        buffer2 = malloc((strlen(buffer) - recc + 1) * sizeof(char));
                                        for (int c = recc; c < strlen(buffer); c++)
                                        {
                                            buffer2[c - recc] = buffer[c];
                                        }
                                        buffer2[strlen(buffer) - recc] = '\0';
                                    }

                                    if (multilinestr[begin - 1] != '/' && multilinestr[begin - 2] != '*' && state == 0)
                                    {
                                        multilinestr[begin] = '\n';
                                        begin += 1;
                                    }

                                    multilinestr[begin] = '\0';

                                    if (multilinestr[begin - 1] == '/' && multilinestr[begin - 2] == '*')
                                    {
                                        multilineflag = 1;
                                        MultilineFound = 0;
                                    }

                                    buffer[j] = '\0';
                                    AnalyseLine(buffer);
                                    memset(buffer, '\0', 200);
                                    printf("%s", multilinestr);
                                    fprintf(html, "<span style='color: green;'>%s</span>", multilinestr);
                                    if (state == 1)
                                    {
                                        AnalyseLine(buffer2);
                                        free(buffer2);
                                        state = 0;
                                    }

                                    if (multilineflag == 1)
                                    {
                                        break;
                                    }
                                }

                                if (j == 0)
                                {
                                    int len = strlen(buffer);
                                    char multilinestr[200];
                                    int begin = 0;
                                    int state = 0;
                                    begin = 0;
                                    int recc;
                                    for (int c = j; c < strlen(buffer); c++)
                                    {
                                        if (buffer[c] != '\r')
                                        {
                                            multilinestr[begin] = buffer[c];
                                            begin += 1;
                                        }
                                        if (buffer[c] == '/' && buffer[c - 1] == '*' && c != strlen(buffer) - 1)
                                        {
                                            state = 1;
                                            recc = c + 1;
                                            break;
                                        }
                                    }

                                    if (multilinestr[begin - 1] != '/' && multilinestr[begin - 2] != '*' && state == 0)
                                    {
                                        multilinestr[begin] = '\n';
                                        begin += 1;
                                    }

                                    char *buffer2;
                                    if (state == 1)
                                    {
                                        buffer2 = malloc((strlen(buffer) - recc + 1) * sizeof(char));
                                        for (int c = recc; c < strlen(buffer); c++)
                                        {
                                            buffer2[c - recc] = buffer[c];
                                        }
                                        buffer2[strlen(buffer) - recc] = '\0';
                                    }
                                    multilinestr[begin] = '\0';

                                    if (multilinestr[begin - 1] == '/' && multilinestr[begin - 2] == '*')
                                    {
                                        multilineflag = 1;
                                        MultilineFound = 0;
                                    }

                                    buffer[j] = '\0';
                                    AnalyseLine(buffer);
                                    memset(buffer, '\0', 200);
                                    printf("%s", multilinestr);
                                    fprintf(html, "<span style='color: green;'>");
                                    for (int p = 0; p < strlen(multilinestr); p++)
                                    {
                                        switch (multilinestr[p])
                                        {
                                        case '<':
                                            fprintf(html, "&lt;");
                                            break;
                                        case '>':
                                            fprintf(html, "&gt;");
                                            break;
                                        case '\"':
                                            fprintf(html, "&quot;");
                                            break;
                                        case '&':
                                            fprintf(html, "&amp;");
                                            break;
                                        case ' ':
                                            fprintf(html, "&nbsp;");
                                            break;
                                        default:
                                            fputc(multilinestr[p], html);
                                            break;
                                        }
                                    }
                                    fprintf(html, "</span>");
                                    if (state == 1)
                                    {
                                        AnalyseLine(buffer2);
                                        free(buffer2);
                                        state = 0;
                                    }

                                    if (multilineflag == 1)
                                    {
                                        break;
                                    }
                                }

                                MultilineFound = 1;
                                break;
                            }
                        }
                    }
                    if (MultilineFound == 1)
                    {
                        multilineflag = 1;
                        while (1)
                        {
                            ch = fgetc(fp);
                            if (ch == EOF)
                            {
                                break;
                            }
                            if (ch != '*')
                            {
                                if (ch != '\r')
                                {
                                    fprintf(html, "<span style='color: green;'>%c</span>", ch);
                                }
                            }
                            if (ch == '*')
                            {
                                if (ch != '\r')
                                {
                                    printf("%c", ch);
                                    fprintf(html, "<span style='color: green;'>%c</span>", ch);
                                }
                                ch = fgetc(fp);
                                if (ch == '/')
                                {
                                    printf("%c", ch);
                                    fprintf(html, "<span style='color: green;'>%c</span>", ch);
                                    MultilineFound = 0;
                                }
                            }
                            if (MultilineFound == 0 || ch == EOF)
                            {
                                break;
                            }
                        }
                    }

                    if (skip == 0 && multilineflag == 0 && SingleLineCommentend == 0)
                    {
                        buffer[strlen(buffer) - 1] = '\0';
                        AnalyseLine(buffer);
                    }
                    multilineflag = 0;
                    SingleLineCommentend = 0;
                    i = 0;

                    if (ch == '\n')
                    {
                        fprintf(html, "\n");
                    }
                    break;
                }
            }
        }

        if (ch == EOF)
        {
            break;
        }
    }
    free(buffer);
    return e_success;
}

Status AnalyseLine(char *buffer)
{
    printf("Buffer is %s\n", buffer);
    OpenBracketcount = 0;
    CloseBracketcount = 0;
    closesquarebracketCount = 0;
    opensquarebracketCount = 0;
    int EndFlag = 0;
    char Word[200];
    int index = 0;
    int j;
    int DoubleQuoteFlag = 0;
    int LiteralClosed = 0;
    char DoubleString[200];
    ;
    int len = strlen(buffer);
    int indarr[len];
    int Literalcount = 0;
    int recorddouble;
    int CharFlag = 0;
    int Charcount = 0;

    for (int i = 0; i < strlen(buffer); i++)
    {
        indarr[i] = -1;
    }

    for (int i = 0; i < strlen(buffer); i++)
    {

        if (buffer[i] == '\n')
        {
            fprintf(html, "\n");
            continue;
        }
        if (buffer[i] == '\r')
        {
            continue;
        }

        for (int l = 0; l < strlen(buffer); l++)
        {
            if (indarr[l] == i)
            {
                DoubleQuoteFlag = 1;
                break;
            }
            else if (indarr[l] == -1)
            {
                continue;
            }
        }

        if (DoubleQuoteFlag == 1)
        {
            DoubleQuoteFlag = 0;
            continue;
        }

        if (buffer[i] == '\'')
        {
            if (CharFlag == 0)
            {
                EndFlag = 0;
            }

            if (buffer[i - 1] == '\\' && buffer[i - 2] == '\\' && EndFlag != 1)
            {
                CharFlag = 1;
                Charcount += 1;
            }
            if (buffer[i - 1] != '\\' && EndFlag != 1)
            {
                CharFlag = 1;
                Charcount += 1;
            }
        }

        if (buffer[i] == '\"' && buffer[i - 1] != '\\' && CharFlag == 0)
        {
            DoubleQuoteFlag = 0;
            int Slashcount = 0;
            Literalcount += 1;
            j = 0;
            int p = j;
            int k = i;
            Word[j] = buffer[k];
            DoubleString[j] = buffer[k];
            indarr[j] = k;
            j += 1;
            k += 1;
            int breakflag = 0;

            for (int f = k; f < strlen(buffer); f++)
            {

                if (buffer[f] == '\\' && Slashcount == 0 && (buffer[f + 1] == '\\' || buffer[f + 1] == '\"'))
                {
                    Slashcount += 1;
                }
                if (buffer[f] == '\\' && buffer[f - 1] == '\\' && Slashcount != 0 && (buffer[f + 1] == '\\' || buffer[f + 1] == '\"'))
                {
                    Slashcount += 1;
                }
                if (buffer[f] == '\\' && buffer[f - 1] == '\\' && Slashcount != 0 && (buffer[f + 1] != '\\' && buffer[f + 1] != '\"'))
                {
                    Slashcount = 0;
                }

                if (buffer[f] == '\"')
                {

                    if (buffer[f - 1] == '\\' && Slashcount % 2 != 0)
                    {
                        Slashcount = 0;
                        continue;
                    }

                    if (buffer[f - 1] != '\\' || (Slashcount % 2 == 0) && buffer[f - 1] == '\\')
                    {
                        Literalcount += 1;
                        Slashcount = 0;
                        breakflag = 1;
                    }
                    recorddouble = f;
                }
                if (breakflag == 1)
                {
                    break;
                }
                DoubleString[p] = k;
                p += 1;
            }
            if (Literalcount % 2 != 0)
            {
                DoubleString[p] = '\0';
                printf("Error : \" missing\n");
            }
            else if (Literalcount % 2 == 0)
            {

                while (k <= recorddouble)
                {
                    Word[j] = buffer[k];
                    indarr[j] = k;
                    j += 1;
                    k += 1;
                    if (k == recorddouble)
                    {
                        Word[j] = buffer[k];
                        LiteralClosed = 1;
                        indarr[j] = k;
                        Word[j + 1] = '\0';
                        if (LiteralClosed == 1)
                        {
                            printf("String Literal : %s\n", Word);
                            fprintf(html, "<span class='string'>");
                            for (int c = 0; c < strlen(Word); c++)
                            {
                                switch (Word[c])
                                {
                                case '<':
                                    fprintf(html, "&lt;");
                                    break;
                                case '>':
                                    fprintf(html, "&gt;");
                                    break;
                                case '\"':
                                    fprintf(html, "&quot;");
                                    break;
                                case '&':
                                    fprintf(html, "&amp;");
                                    break;
                                case ' ':
                                    fprintf(html, "&nbsp;");
                                    break;
                                default:
                                    fputc(Word[c], html);
                                    break;
                                }
                            }
                            fprintf(html, "</span>");
                        }
                        if (LiteralClosed == 0)
                        {
                            printf("Error : \" missing\n");
                        }
                        break;
                    }
                }
            }
        }
        else
        {
            Status collect;
            if (CharFlag == 0)
            {
                collect = CheckCharIsOperatorOrSpecialCharacters(buffer[i], 1);
            }
            if (CharFlag == 1)
            {
                collect = e_failure;
            }
            if (CharFlag == 1 && buffer[i] == ' ')
            {
                Word[index] = buffer[i];
            }

            if (collect == e_failure && buffer[i] != ' ' || (buffer[i] == ' ' && CharFlag == 1))
            {
                Word[index] = buffer[i];
                if (Charcount == 2)
                {
                    Word[index + 1] = '\0';
                    CompareWord(Word, keywords, operators);
                    CharFlag = 0;
                    EndFlag = 1;
                    Charcount = 0;
                    index = 0;
                    continue;
                }

                collect = CheckCharIsOperatorOrSpecialCharacters(buffer[i + 1], 0);
                index += 1;

                if ((collect == e_success || buffer[i + 1] == ' ') && CharFlag == 0)
                {
                    Word[index] = '\0';
                    CompareWord(Word, keywords, operators);
                    index = 0;
                }

                if (i == strlen(buffer) - 1)
                {
                    if (Word[index - 1] == '\r' || Word[index - 1] == '\n')
                    {
                        Word[index - 1] = '\0'; // strip newline/CR
                    }

                    Word[index] = '\0';
                    CharFlag = 0;
                    Charcount = 0;
                    CompareWord(Word, keywords, operators);
                    index = 0;
                }
            }
            else if (collect == e_success || buffer[i] == ' ')
            {

                if (buffer[i] == ' ')
                {
                    fprintf(html, "&nbsp;");
                }
                index = 0;
            }
        }
    }
    if (index > 0)
    {
        if (Word[index - 1] == '\r' || Word[index - 1] == '\n')
        {
            Word[index - 1] = '\0';
        }
        Word[index] = '\0';
        CompareWord(Word, keywords, operators);
        index = 0;
    }

    if (OpenBracketcount != CloseBracketcount)
    {
        if (OpenBracketcount > CloseBracketcount)
        {
            printf("Error : ) missing\n");
        }
        else if (OpenBracketcount < CloseBracketcount)
        {
            printf("Error : ( missing\n");
        }
    }

    if (opensquarebracketCount != closesquarebracketCount)
    {
        if (opensquarebracketCount > closesquarebracketCount)
        {
            printf("Error : ] missing\n");
        }
        else if (opensquarebracketCount < closesquarebracketCount)
        {
            printf("Error : [ missing\n");
        }
    }
    OpenBracketcount = 0;
    CloseBracketcount = 0;
    closesquarebracketCount = 0;
    opensquarebracketCount = 0;
}
Status CheckCharIsOperatorOrSpecialCharacters(char ch, int num)
{
    for (int i = 0; i < strlen(operators); i++)
    {
        if (ch == operators[i])
        {
            if (num == 1)
            {
                printf("Operator : %c\n", ch);
                fprintf(html, "<span class='operator'>%c</span>", ch);
            }
            return e_success;
        }
    }
    for (int i = 0; i < strlen(specialCharacters); i++)
    {
        if (ch == specialCharacters[i])
        {
            switch (specialCharacters[i])
            {
            case '{':
                if (num == 1)
                {
                    OpencurlyBracketcount += 1;
                }
                break;
            case '}':
                if (num == 1)
                {
                    ClosecurlyBracketcount += 1;
                }
                break;

            case '(':
                if (num == 1)
                {
                    OpenBracketcount += 1;
                }
                break;
            case ')':
                if (num == 1)
                {
                    CloseBracketcount += 1;
                }
                break;

            case '[':
                if (num == 1)
                {
                    opensquarebracketCount += 1;
                }
                break;
            case ']':
                if (num == 1)
                {
                    closesquarebracketCount += 1;
                }
                break;

            default:
                break;
            }
            if (num == 1)
            {
                printf("Special Character : %c\n", ch);
                fprintf(html, "<span class='special'>%c</span>", ch);
            }
            return e_success;
        }
    }
    return e_failure;
}

Status CompareWord(char *Word, const char *keywords[], const char *operators)
{
    for (int i = 0; i < 32; i++)
    {
        if (strcmp(Word, keywords[i]) == 0)
        {
            printf("Keyword : %s\n", Word);
            fprintf(html, "<span class='keyword'>%s</span>", Word);
            return e_success;
        }
    }
    int Flag = 1;
    int DotFlag = 0;
    int Dotcount = 0;
    int Errorflag = 0;
    if (Word[0] >= '0' && Word[0] <= '9')
    {

        for (int i = 1; i < strlen(Word); i++)
        {
            if (Word[i] == '.')
            {
                DotFlag = 1;
                Dotcount += 1;
            }
            else if (Word[i] >= '0' && Word[i] <= '9')
            {
                Flag = Flag && 1;
            }
            else if (!(Word[i] >= '0' && Word[i] <= '9'))
            {
                Flag = Flag && 0;
                Errorflag = 1;
            }
        }

        if (Errorflag == 1 && DotFlag == 1 && Dotcount == 1)
        {
            int Error = 0;

            if (Word[strlen(Word) - 1] == 'L' || (Word[strlen(Word) - 1] == 'f'))
            {
                for (int i = 0; i < strlen(Word) - 1; i++)
                {
                    if (Word[i] == '.')
                    {
                        continue;
                    }
                    else if (Word[i] == 'e')
                    {
                        continue;
                    }
                    if (!(Word[i] >= '0' && Word[i] <= '9'))
                    {
                        Error = 1;
                    }
                }
                if (Error == 0)
                {
                    printf("Floating Point constant : %s\n", Word);
                    fprintf(html, "<span class='constant'>%s</span>", Word);
                }
                else if (Error == 1)
                {
                    printf("Error in floating point\n");
                }
            }
        }
        else if (Flag == 1 && DotFlag == 0)
        {
            printf("Integer Constant : %s\n", Word);
            fprintf(html, "<span class='constant'>%s</span>", Word);
        }
        else if (DotFlag == 1 && Dotcount == 1 && Errorflag == 0)
        {
            printf("Floating point constant : %s\n", Word);
            fprintf(html, "<span class='constant'>%s</span>", Word);
        }
        else if (Errorflag == 1)
        {
            printf("Error in Identifiers \n");
        }
    }
    else if (Word[0] == '\'')
    {
        if (strlen(Word) == 4 && Word[1] == '\\' && Word[3] == '\'')
        {
            char esc[3];
            esc[0] = Word[1];
            esc[1] = Word[2];
            esc[2] = '\0';
            for (int i = 0; i < 11; i++)
            {
                if (strcmp(esc, validCharConstants[i]) == 0)
                {
                    printf("Character constant : %s\n", Word);
                    fprintf(html, "<span class='constant'>%s</span>", Word);
                    break;
                }
            }
        }
        else if (strlen(Word) == 3 && Word[2] == '\'')
        {
            printf("Character constant %s\n", Word);
            fprintf(html, "<span class='constant'>%s</span>", Word);
        }
        else
        {
            printf("Error in character constant ");
        }
    }
    else if ((Word[0] >= 'a' && Word[0] <= 'z') || (Word[0] >= 'A' && Word[0] <= 'Z') || (Word[0] == '_'))
    {
        int l = strlen(Word);
        int first = 0;
        int last = 0;
        int Errorflag = 0;

        for (int i = 0; i < l; i++)
        {
            if ((!(Word[i] >= 'a' && Word[i] <= 'z')) && (!(Word[i] >= 'A' && Word[i] <= 'Z')) && (!(Word[i] >= '0' && Word[i] <= '9')) && Word[i] != '_' && Word[i] != '.')
            {
                Errorflag = 1;
            }

            if (Word[i] == '.' || i == l - 1)
            {
                if (Word[i] == '.')
                {
                    last = i - 1;
                    char subWord[(last - first) + 2];

                    for (int j = first; j <= last; j++)
                    {
                        subWord[j - first] = Word[j];
                    }
                    subWord[(last - first) + 1] = '\0';
                    printf("subword is %s\n", subWord);
                    CompareSubWord(subWord, keywords, operators);
                }
                else if (i == l - 1)
                {
                    last = l - 1;
                    char subWord[(last - first) + 2];

                    for (int j = first; j <= last; j++)
                    {
                        subWord[j - first] = Word[j];
                    }
                    subWord[(last - first) + 1] = '\0';
                    CompareSubWord(subWord, keywords, operators);
                }

                if (Errorflag == 0)
                {
                }
                else if (Errorflag == 1)
                {
                    printf("Error in Identifiers");
                }
                printf("\n");
                if (Word[i] == '.')
                {
                    printf("Special character : .\n");
                    fprintf(html, "<span class='special'>.</span>");
                    first = i + 1;
                    Errorflag = 0;
                }
            }
        }
    }
}

Status CompareSubWord(char *Word, const char *keywords[], const char *operators)
{
    for (int i = 0; i < 32; i++)
    {
        if (strcmp(Word, keywords[i]) == 0)
        {
            printf("Keyword : %s\n", Word);
            fprintf(html, "<span class='keyword'>%s</span>", Word);
            return e_success;
        }
    }
    int Flag = 1;
    int DotFlag = 0;
    int Dotcount = 0;
    int Errorflag = 0;

    if (strlen(Word) == 1 && Word[0] == '.')
    {
        printf("Special Character : .\n");
        fprintf(html, "<span class='special'>.</span>");
    }
    if (Word[0] >= '0' && Word[0] <= '9')
    {

        for (int i = 1; i < strlen(Word); i++)
        {
            if (Word[i] == '.')
            {
                DotFlag = 1;
                Dotcount += 1;
            }
            else if (Word[i] >= '0' && Word[i] <= '9')
            {
                Flag = Flag && 1;
            }
            else if (!(Word[i] >= '0' && Word[i] <= '9'))
            {
                Flag = Flag && 0;
                Errorflag = 1;
            }
        }

        if (Flag == 1 && DotFlag == 0)
        {
            printf("Integer Constant : %s\n", Word);
            fprintf(html, "<span class='constant'>%s</span>", Word);
        }
        else if (DotFlag == 1 && Dotcount == 1 && Errorflag == 0)
        {
            printf("Floating point constant : %s\n", Word);
        }
        else if (Errorflag == 1)
        {
            printf("Error in Identifiers \n");
        }
    }
    else if (Word[0] == '\'')
    {
        if (strlen(Word) == 4 && Word[1] == '\\' && Word[3] == '\'')
        {
            char esc[3];
            esc[0] = Word[1];
            esc[1] = Word[2];
            esc[2] = '\0';
            for (int i = 0; i < 11; i++)
            {
                if (strcmp(esc, validCharConstants[i]) == 0)
                {
                    printf("Character constant : %s\n", Word);
                    fprintf(html, "<span class='constant'>%s</span>", Word);
                    break;
                }
            }
        }
        else if (strlen(Word) == 3 && Word[2] == '\'')
        {
            printf("Character constant %s\n", Word);
            fprintf(html, "<span class='constant'>%s</span>", Word);
        }
        else
        {
            printf("Error in character constant ");
        }
    }
    else if ((Word[0] >= 'a' && Word[0] <= 'z') || (Word[0] >= 'A' && Word[0] <= 'Z') || (Word[0] == '_'))
    {

        int l = strlen(Word);
        int first = 0;
        int last = 0;
        int Errorflag = 0;

        for (int i = 0; i < l; i++)
        {
            if ((!(Word[i] >= 'a' && Word[i] <= 'z')) && (!(Word[i] >= 'A' && Word[i] <= 'Z')) && (!(Word[i] >= '0' && Word[i] <= '9')) && Word[i] != '_' && Word[i] != '.')
            {
                Errorflag = 1;
            }
        }

        if (Errorflag == 0)
        {
            printf("Identifiers : ");
            printf("%s\n", Word);
            fprintf(html, "<span class='identifier'>%s</span>", Word);
        }
        else if (Errorflag == 1)
        {
            printf("Error in Identifiers");
        }
    }
}

Status ProcessPreprocessor(char *buffer)
{
    int Purpleflag = 0;
    int Brownflag = 0;
    int c = 0;
    for (int i = 0; i < strlen(buffer); i++)
    {
        if (buffer[i] == '\r')
        {
            continue;
        }

        if (buffer[i] == '#')
        {
            fprintf(html, "<span style='color: purple;'>%c</span>", buffer[i]);
            Purpleflag = 1;
        }
        if (Purpleflag == 1 && buffer[i] != '#' && buffer[i] != '<' && buffer[i] != '>' && buffer[i] != '\"')
        {
            fprintf(html, "<span style='color: purple;'>%c</span>", buffer[i]);
        }
        if ((buffer[i] == '<' || buffer[i] == '\"') && Purpleflag == 1)
        {
            if (buffer[i] == '\"')
            {
                c = i;
            }
            Brownflag = 1;
            Purpleflag = 0;
            fprintf(html, "<span style='color: brown;'>%c</span>", buffer[i]);
        }

        if (buffer[i] == '>' || (buffer[i] == '\"' && Brownflag == 1 && i != c))
        {
            Brownflag = 0;
            fprintf(html, "<span style='color: brown;'>%c</span>", buffer[i]);
        }
        if (Brownflag == 1 && (buffer[i] != '<' && buffer[i] != '>' && buffer[i] != '\"'))
        {
            fprintf(html, "<span style='color: brown;'>%c</span>", buffer[i]);
        }
    }
}