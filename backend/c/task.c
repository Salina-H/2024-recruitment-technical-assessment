#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRING_LEN 50
#define MAX_CATEGORIES 10

struct File {
    int id;
    char name[MAX_STRING_LEN];
    char categories[MAX_CATEGORIES][MAX_STRING_LEN];
    int numCategories;
    int parent;
    int size;
};
struct Category {
    char *name;
    int freq;
    struct Category *next;
};

struct Category *search_category (struct Category *head, char *category);
struct Category *insert_category (struct Category *head, struct Category *insert);
struct Category *create_category (char *category_name);

int find_sum(struct File *files, int numFiles, struct File file);
/**
 * Task 1
 * Returned list should contain copies of the names e.g. through strdup()
 */
char **leafFiles(struct File *files, int numFiles, int *numLeafFiles) {
    *numLeafFiles = 0;
    int non_leaf [numFiles];
    // find all the non-leaf node ids
    for (int i = 0; i < numFiles; i++) {
        non_leaf[i] = files[i].parent;
    }
    // find all the names of the non-leaf nodes based off id
    bool is_leaf = true;
    char **list = NULL;
    for (int i = 0; i < numFiles; i++) {
        for (int j = 0; j < numFiles; j++) {
            if (files[i].id == non_leaf[j]) {
                is_leaf = false;
                break;
            }
        }
        if (is_leaf) {
            list = realloc(list, (*numLeafFiles + 1) * sizeof(char *));
            list[*numLeafFiles] = strdup(files[i].name);
            (*numLeafFiles)++;
        } else {
            is_leaf = true;
        }
    }
    return list;
}

/**
 * Task 2
 * Returned list should contain copies of the categories e.g. through strdup()
 */
char **kLargestCategories(struct File *files, int numFiles, int k, int *numReturned) {
    // create a linked list of all the categories in sorted order
    struct Category *head = NULL;
    int categories_count = 0;
    for (int i = 0; i < numFiles; i++) {
        for (int category = 0; category < files[i].numCategories; category++) {
            // search in linked list for the category
            struct Category *found = search_category(head, files[i].categories[category]);
            // if the category doesn't already exist
            if (found == NULL) {
                categories_count++;
                struct Category *new = create_category(files[i].categories[category]);
                //add to the end of the list
                head = insert_category(head, new);
            // else add the category and sort
            } else {
                (found->freq)++;
                // disconnect
                struct Category *prev = NULL;
                struct Category *cur = head;
                while (cur != found) {
                    prev = cur;
                    cur = cur->next;
                }
                // if found is at the head
                if (prev == NULL) {
                    head = head->next;
                } else {
                    prev->next = found->next;
                }
                found->next = NULL;

                head = insert_category(head, found);
            }
        }
    }
    // create list of category names to return + free list
    struct Category *freed = head;
    struct Category *transfer = head;
    *numReturned = 0;
    char **list = NULL;
    while (transfer != NULL) {
        if (k > 0) {
            list = realloc(list, (*numReturned + 1) * sizeof(char *));
            list[*numReturned] = transfer->name;
            (*numReturned)++;
            k--;
        } else {
            free(freed->name);
        }
        transfer = transfer->next;
        free(freed);
        freed = transfer;
    }
    return list;
}
// return NULL or found category
struct Category *search_category (struct Category *head, char *category) {
    struct Category *it = head;
    for (; it != NULL && strcmp(it->name, category) != 0; it = it->next);
    return it;
}
// returns head of the list
// inserts insert freq high to low then a - z
struct Category *insert_category (struct Category *head, struct Category *insert) {
    if (head == NULL) {
        return insert;
    }
    struct Category *prev = NULL;
    struct Category *cur = head;
    while (cur != NULL && (cur->freq > insert->freq || 
        (cur->freq == insert->freq && strcmp(cur->name, insert->name) < 0))
    ) {
        prev = cur;
        cur = cur->next;
    }
    insert->next = cur;
    if (prev == NULL) {
        head = insert;
    } else {
        prev->next = insert;
    }
    return head;
}
struct Category *create_category (char *category_name) {
    struct Category *new = malloc(sizeof(struct Category));
    new->name = strdup(category_name);
    new->freq = 1;
    new->next = NULL;
    return new;
}

// why am i coding this in C tbh

/**
 * Task 3
 */
// assuming each file can only have at most 1 parent
int largestFileSize(struct File *files, int numFiles) {
    int max = 0;
    for (int i = 0; i < numFiles; i++) {
        // if the root of a tree is found
        if (files[i].parent == -1) {
            // recursively find the sum of a file tree
            int sum = find_sum(files, numFiles, files[i]);
            if (max < sum) max = sum;
        }
    }
    return max;
}

// id is id of the root
int find_sum(struct File *files, int numFiles, struct File file) {
    int sum = file.size;
    for (int i = 0; i < numFiles; i++) {
        if (files[i].parent == file.id) {
            sum += find_sum(files, numFiles, files[i]);
        }
    }
    return sum;
}

int qsortStrcmp(const void *a, const void *b) {
    return strcmp(*(char **)a, *(char **)b);
}

int main(void) {
    struct File testFiles[] = {
        { .id = 1, .name = "Document.txt", .categories = {"Documents"}, .numCategories = 1, .parent = 3, .size = 1024 },
        { .id = 2, .name = "Image.jpg", .categories = {"Media", "Photos"}, .numCategories = 2, .parent = 34, .size = 2048 },
        { .id = 3, .name = "Folder", .categories = {"Folder"}, .numCategories = 1, .parent = -1, .size = 0 },
        { .id = 5, .name = "Spreadsheet.xlsx", .categories = {"Documents", "Excel"}, .numCategories = 2, .parent = 3, .size = 4096 },
        { .id = 8, .name = "Backup.zip", .categories = {"Backup"}, .numCategories = 1, .parent = 233, .size = 8192 },
        { .id = 13, .name = "Presentation.pptx", .categories = {"Documents", "Presentation"}, .numCategories = 2, .parent = 3, .size = 3072 },
        { .id = 21, .name = "Video.mp4", .categories = {"Media", "Videos"}, .numCategories = 2, .parent = 34, .size = 6144 },
        { .id = 34, .name = "Folder2", .categories = {"Folder"}, .numCategories = 1, .parent = 3, .size = 0 },
        { .id = 55, .name = "Code.py", .categories = {"Programming"}, .numCategories = 1, .parent = -1, .size = 1536 },
        { .id = 89, .name = "Audio.mp3", .categories = {"Media", "Audio"}, .numCategories = 2, .parent = 34, .size = 2560 },
        { .id = 144, .name = "Spreadsheet2.xlsx", .categories = {"Documents", "Excel"}, .numCategories = 2, .parent = 3, .size = 2048 },
        { .id = 233, .name = "Folder3", .categories = {"Folder"}, .numCategories = 1, .parent = -1, .size = 4096 },
    };

    int numLeafFiles;
    char *expectedLeafFiles[] = {
        "Audio.mp3",
        "Backup.zip",
        "Code.py",
        "Document.txt",
        "Image.jpg",
        "Presentation.pptx",
        "Spreadsheet.xlsx",
        "Spreadsheet2.xlsx",
        "Video.mp4"
    };
    char **returnedLeafFiles = leafFiles(testFiles, 12, &numLeafFiles);
    qsort(returnedLeafFiles, numLeafFiles, sizeof(char *), &qsortStrcmp);

    assert(numLeafFiles == 9);
    for (int i = 0; i < 9; i++) {
        assert(strcmp(returnedLeafFiles[i], expectedLeafFiles[i]) == 0);
        free(returnedLeafFiles[i]);
    }
    free(returnedLeafFiles);

    int numCategories;
    char *expectedCategories[] = {"Documents", "Folder", "Media"};
    char **returnedCategories = kLargestCategories(testFiles, 12, 3, &numCategories);
    assert(numCategories == 3);
        for (int i = 0; i < 3; i++) {
        assert(strcmp(returnedCategories[i], expectedCategories[i]) == 0);
        free(returnedCategories[i]);
    }
    free(returnedCategories);
    assert(largestFileSize(testFiles, 12) == 20992);
}