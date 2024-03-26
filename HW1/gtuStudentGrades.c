#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>


void writeLog(const char *logMessage) {
int log_fd = open("action.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
if (log_fd == -1) {
perror("Error opening log file");
return;
}

char timeBuffer[128];
time_t now = time(NULL);
struct tm *tm_info = localtime(&now);
strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", tm_info);

if (dprintf(log_fd, "[%s] %s\n", timeBuffer, logMessage) == -1) {
    perror("Error writing to log file");
}

close(log_fd);
}




//Ayrılan Kısım

void initializeFile(const char *filename) {
    int fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fd == -1) {
        perror("Error creating file");
    } else {
        printf("%s initialized.\n", filename);
        close(fd);
    }
}

void addStudentGrade(const char *filename, const char *name, const char *grade) {
    pid_t pid = fork();
    
    if (pid == -1) {
        // Error handling
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd == -1) {
            perror("Error opening/creating the file");
            exit(EXIT_FAILURE); // Use exit(EXIT_FAILURE) in child to indicate failure
        }
        
        if (dprintf(fd, "%s, %s\n", name, grade) < 0) {
            perror("Error writing to the file");
            close(fd);
            exit(EXIT_FAILURE); // Exit child with failure status if writing fails
        } else {
            printf("Grade added for %s\n", name);
            close(fd);
            exit(EXIT_SUCCESS); // Exit child successfully after adding grade
        }
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) {
            // Optionally handle successful addition
            writeLog("Grade added successfully");
        } else {
            // Handle failure
            fprintf(stderr, "Failed to add grade\n");
        }
    }
}


void printUsage() {
    printf("Usage:\n");
    printf("  gtuStudentGrades <filename> - Initialize system with file\n");
    printf("  addStudentGrade \"Name Surname\" \"Grade\" - Add a new grade\n");
    printf("  showAll - Display all grades\n");

}


void showAll(const char *filename) {
    pid_t pid = fork();

    if (pid == -1) {
        // If fork fails
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            perror("Error opening file");
            exit(EXIT_FAILURE); 
        }

        char buffer[256];
        ssize_t bytes_read;
        while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            printf("%s", buffer); 
        }
        
        close(fd);
        exit(EXIT_SUCCESS); 
    } else {
 
        int status;
        waitpid(pid, &status, 0); // Wait for the child process to finish

        // After child process completes, log the action
        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) {
            writeLog("Displayed all student grades successfully.");
        } else {
            // Handle potential error or unsuccessful display attempt
            writeLog("Failed to display all student grades.");
        }
    }
}


void listSome(const char *filename, int numOfEntries, int pageNumber) {
    pid_t pid = fork();

    if (pid == -1) {
        // If fork fails
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            perror("Error opening file");
            exit(EXIT_FAILURE); // Exit with failure if unable to open the file
        }

        char buffer[1024];
        int currentLine = 1, printedLines = 0;
        int startLine = (pageNumber - 1) * numOfEntries + 1;
        int endLine = startLine + numOfEntries - 1;

        ssize_t bytes_read;
        while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            char *ptr = buffer;
            char *lineStart = ptr;

            while (*ptr) {
                if (*ptr == '\n' || *(ptr + 1) == '\0') {
                    if (*(ptr + 1) == '\0' && *ptr != '\n') ptr++; // Include last line without newline character
                    *ptr = '\0'; // Null-terminate the line

                    if (currentLine >= startLine && currentLine <= endLine) {
                        printf("%s\n", lineStart);
                        printedLines++;
                        if (printedLines >= numOfEntries) break; // Break if we've printed enough lines
                    }

                    lineStart = ptr + 1; 
                    currentLine++;
                }
                ptr++;
            }
            if (printedLines >= numOfEntries) {
                    break; 
            } 
        }

        close(fd);
        exit(EXIT_SUCCESS); 
    } else {

        int status;
        waitpid(pid, &status, 0); 

        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) {
 
        } else {
         
        }
    }
}



void listGrades(const char *filename) {
    listSome(filename, 1, 5); 
}





void searchStudent(const char *filename, const char *searchName) {
    pid_t pid = fork();
    
    if (pid == -1) {
        // Error handling if fork fails
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            perror("Error opening the file");
            exit(EXIT_FAILURE); 
        }

        char line[256];
        ssize_t nread;
        int found = 0;


        while ((nread = read(fd, line, sizeof(line) - 1)) > 0) {
            line[nread] = '\0'; 
            char *start = line;
            char *end;
            while ((end = strchr(start, '\n')) != NULL) {
                *end = '\0'; // Null-terminate the current line

                if (strstr(start, searchName) != NULL) {
                    printf("%s\n", start);
                    found = 1;
                    break; 
                }

                start = end + 1; 
            }
            if (found){
                break; 
            }
            
  
        }

        close(fd);
        char logMessage[512];
        if (found) {
            snprintf(logMessage, sizeof(logMessage), "Search completed. Student found: %s", searchName);
        } else {
            snprintf(logMessage, sizeof(logMessage), "Search completed. Student not found: %s", searchName);
        }
        writeLog(logMessage); 

        exit(found ? EXIT_SUCCESS : EXIT_FAILURE); 
    } else {
        int status;
        waitpid(pid, &status, 0); 

        if (WIFEXITED(status)) {
            int exitStatus = WEXITSTATUS(status);
            if (exitStatus == EXIT_FAILURE) {
                writeLog("Search operation completed. Student not found.");
            } else {
                
            }
        }
    }
}


void sortStudentGrades(const char *filename) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0); 
        if (WIFEXITED(status)) {
            printf("Sorting completed.\n");
        }
    } else {
        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            perror("Error opening file for reading");
            exit(EXIT_FAILURE);
        }
        const int maxLineLength = 260;
        const int maxLines = 100;
        
        char *lines[maxLines];
        char buffer[maxLineLength * maxLines];
        ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
        if (bytesRead == -1) {
            perror("Error reading file");
            close(fd);
            exit(EXIT_FAILURE);
        }
        close(fd);

    
        char *line = strtok(buffer, "\n");
            int lineCount = 0;
        while (line != NULL && lineCount < maxLines) {
            lines[lineCount++] = strdup(line); 
            line = strtok(NULL, "\n");
        }

      
        for (int i = 0; i < lineCount - 1; ++i) {
            for (int j = 0; j < lineCount - i - 1; ++j) {
                if (strcmp(lines[j], lines[j + 1]) > 0) {
                    char *temp = lines[j];
                    lines[j] = lines[j + 1];
                    lines[j + 1] = temp;
                }
            }
        }

        fd = open(filename, O_WRONLY | O_TRUNC);
        if (fd == -1) {
            perror("Error opening file for writing");
            for (int i = 0; i < lineCount; ++i) free(lines[i]); 
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < lineCount; ++i) {
            write(fd, lines[i], strlen(lines[i]));
            write(fd, "\n", 1);
            free(lines[i]); 
        }
        close(fd);

        exit(EXIT_SUCCESS); 
    }
}

int main() {
    char command[256], filename[256];
    int fileInitialized = 0;

    printf("GTU Student Grade Management System\n");
    printUsage();
    while (1) {
        printf("> ");
        if (!fgets(command, sizeof(command), stdin)) {
            continue;
        }
        command[strcspn(command, "\n")] = 0; 

        if (strncmp(command, "gtuStudentGrades", 16) == 0) {
            sscanf(command, "gtuStudentGrades %255s", filename);
            int fd = open(filename, O_CREAT | O_WRONLY, 0644);
            if (fd == -1) {
                perror("Error checking/creating the file");
                continue;
            }
            printf("%s is ready for use.\n", filename);
            fileInitialized = 1;
            close(fd);
        }
        else if (strncmp(command, "addStudentGrade", 15) == 0 && fileInitialized) {
            char name[256], grade[3];

            if (sscanf(command, "addStudentGrade \"%255[^\"]\" \"%2s\"", name, grade) == 2) {
                addStudentGrade(filename, name, grade);
            } else {
                printf("%s %s\n",name,grade);
                printf("Invalid command format. Correct format: addStudentGrade \"Name Surname\" \"Grade\"\n");
            }
        } else if (strncmp(command, "searchStudent", 13) == 0 && fileInitialized) {
            char searchName[256];
            if (sscanf(command, "searchStudent \"%255[^\"]\"", searchName) == 1) {
                searchStudent(filename, searchName);
            } else {
                printf("Invalid command format. Correct format: searchStudent \"Name Surname\"\n");
            }
        } else if(strcmp(command,"showAll")==0){
            showAll(filename);
        }else if (strncmp(command, "listSome", 8) == 0 && fileInitialized) {
            char* remainingCommand = command + 9; // Move past "listSome" and the space
            char* numStr = strtok(remainingCommand, " ");
            char* pageStr = strtok(NULL, " ");
    if (numStr != NULL && pageStr != NULL) {
        printf("Parsing for listing: %s entries, page %s\n", numStr, pageStr);
        int pageNumber = atoi(pageStr);
        int numOfEntries = atoi(numStr);
        if (numOfEntries > 0 && pageNumber > 0) { // Simple validation
            listSome(filename, numOfEntries, pageNumber);
        } else {
            printf("Number of entries and page number must be positive integers.\n");
        }
    } else {
        printf("Invalid command format for listSome. Expected format: listSome numOfEntries pageNumber\n");
    }
}else if (strncmp(command, "sortAll", 7) == 0 && fileInitialized) {
    sortStudentGrades(filename);
}

         else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            printf("Unknown command or file not initialized.\n");
        }
    }
    return 0;
}




