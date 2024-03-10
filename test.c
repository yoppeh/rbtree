// test.c

#include <fcntl.h> 
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "rbtree.h"

typedef struct list_node_t {
    struct list_node_t *next;
    char *key;
} list_node_t;

static list_node_t *delete_list = NULL;
static list_node_t *no_delete_list = NULL;
static char *words = NULL;
static rbtree_t *deleted_tree = NULL;
static rbtree_t *randomized_tree = NULL;
static rbtree_t *tmp_tree = NULL;
static rbtree_t *word_tree = NULL;
static int missing_count = 0;
static int tmp_count = 0;
static int word_count = 0;
static int delete_count = 0;
static int to_delete_count = 0;
static char *min = NULL;
static char *max = NULL;

static int cmp_func_int(void *a, void *b);
static int load_words(void);
static int del_traversal_cb(rbtree_node_t *node);
static int tmp_traversal_cb(rbtree_node_t *node);
static int in_randomized_traversal_cb(rbtree_node_t *node);
static int in_word_traversal_cb(rbtree_node_t *node);

int main(int ac, char **av) {
    deleted_tree = rbtree_new((rbtree_key_compare_func_t)strcmp, NULL);
    randomized_tree = rbtree_new((rbtree_key_compare_func_t)strcmp, NULL);
    tmp_tree = rbtree_new(cmp_func_int, NULL);
    word_tree = rbtree_new((rbtree_key_compare_func_t)strcmp, NULL);
    rbtree_node_t *n;
    int rc = 1;
    if (deleted_tree == NULL || randomized_tree == NULL || tmp_tree == NULL || word_tree == NULL) {
        fprintf(stderr, "error allocating tree structure\n");
        goto end;
    }
    printf("loading words.txt to word_tree and tmp_tree... ");
    fflush(stdout);
    if (load_words()) {
        goto end;
    }
    printf("ok!\n");
    printf("loaded %i words into word tree (tree says %li)\n", word_count, word_tree->node_count);
    printf("loaded %i words into temporary tree (tree says %li)\n", tmp_count, tmp_tree->node_count);
    if (word_tree->node_count != tmp_tree->node_count) {
        printf("mismatch in word count between temporary tree (%li) and word tree (%li)\n", tmp_tree->node_count, word_tree->node_count);
        goto end;
    } else {
        printf("word counts match!\n");
    }
    printf("transferring temporary tree to randomized tree... ");
    fflush(stdout);
    rbtree_traverse_ascending(tmp_tree, NULL, tmp_traversal_cb);
    printf("ok!\n");
    printf("deleting all nodes in temporary tree... ");
    rbtree_delete(tmp_tree, NULL);
    tmp_tree = rbtree_new((rbtree_key_compare_func_t)strcmp, NULL);
    printf("ok!\n");
    printf("checking that each word in word tree is also in randomized tree... ");
    fflush(stdout);
    rbtree_traverse_ascending(word_tree, NULL, in_randomized_traversal_cb);
    if (missing_count == 0) {
        printf("ok!\n");
    }
    printf("checking that each word in randomized tree is also in word tree... ");
    fflush(stdout);
    rbtree_traverse_ascending(randomized_tree, NULL, in_word_traversal_cb);
    if (missing_count == 0) {
        printf("ok!\n");
    }
    printf("checking min... ");
    n = rbtree_minimum(randomized_tree, NULL);
    if (strcmp(min, (char *)n->key) != 0) {
        printf("minimum node in randomized tree returns \"%s\", minimum found during load is \"%s\"\n", (char *)n->key, min);
    } else {
        printf("ok!\n");
    }
    printf("checking max... ");
    n = rbtree_maximum(randomized_tree, NULL);
    if (strcmp(max, (char *)n->key) != 0) {
        printf("maximum node in randomized tree returns \"%s\", maximum found during load is \"%s\"\n", (char *)n->key, min);
    } else {
        printf("ok!\n");
    }
    printf("checking key retrieval...\n");
    char **keys = rbtree_get_keys(randomized_tree);
    int i = 0;
    int count = 0;
    for (i = 0; keys[i] != NULL; i++) {
        rbtree_node_t *n = rbtree_lookup(randomized_tree, keys[i]);
        if (n == NULL) {
            printf("key \"%s\" not found in randomized tree\n", keys[i]);
        } else {
            count++;
        }
    }
    printf("found %d keys, %d confirmed\n", i, count);
    printf("%i nodes to delete...\n", to_delete_count);
    printf("deleting approximately half of words in randomized tree...");
    fflush(stdout);
    list_node_t *l = delete_list;
    while (l) {
        rbtree_node_t *n = rbtree_lookup(randomized_tree, l->key);
        if (n != NULL) {
            rbtree_delete_node(randomized_tree, n);
            delete_count++;
        } else {
            printf("hmmm, couldn't delete missing key \"%s\"\n", l->key);
        }
        l = l->next;
    }
    printf("ok!\n");
    printf("deleted %i nodes\n", delete_count);
    printf("checking that no deleted nodes are left in tree\n");
    l = delete_list;
    while (l) {
        if (rbtree_lookup(randomized_tree, l->key) != NULL) {
            printf("node \"%s\" should have been deleted but wasn't\n", l->key);
        } else {
            delete_count--;
        }
        l = l->next;
    }
    printf("%i nodes weren't properly deleted\n", delete_count);
    printf("checking that all non-deleted nodes are still in tree\n");
    l = no_delete_list;
    delete_count = 0;
    while (l) {
        if (rbtree_lookup(randomized_tree, l->key) == NULL) {
            printf("node \"%s\" should not have been deleted but was\n", l->key);
            delete_count++;
        }
        l = l->next;
    }
    printf("%i nodes improperly deleted\n", delete_count);
    rc = 0;
end:
    if (no_delete_list != NULL) {
        l = no_delete_list;
        while (l) {
            no_delete_list = l->next;
            free(l);
            l = no_delete_list;
        }
        no_delete_list = NULL;
    }
    if (delete_list != NULL) {
        l = delete_list;
        while (l) {
            delete_list = l->next;
            free(l);
            l = delete_list;
        }
        delete_list = NULL;
    }
    if (word_tree != NULL) {
        rbtree_free(word_tree);
    }
    if (tmp_tree != NULL) {
        rbtree_free(tmp_tree);
    }
    if (randomized_tree != NULL) {
        rbtree_free(randomized_tree);
    }
    if (deleted_tree != NULL) {
        rbtree_free(deleted_tree);
    }
    if (words != NULL) {
        free(words);
    }
    return(rc);
}

static int cmp_func_int(void *a, void *b) {
    if ((uint64_t)a < (uint64_t)b) {
        return(-1);
    } else if ((uint64_t)a == (uint64_t)b) {
        return(0);
    } else {
        return(1);
    }
}

static int tmp_traversal_cb(rbtree_node_t *node) {
    rbtree_node_t *n = rbtree_insert(randomized_tree, node->data);
    return(0);
}

static int in_randomized_traversal_cb(rbtree_node_t *node) {
    rbtree_node_t *n = rbtree_lookup(randomized_tree, node->key);
    if (n == NULL) {
        printf("\"%s\" not found in randomized tree!\n", (char *)node->key);
        missing_count++;
    }
    return(0);
}

static int in_word_traversal_cb(rbtree_node_t *node) {
    rbtree_node_t *n = rbtree_lookup(word_tree, node->key);
    if (n == NULL) {
        printf("\"%s\" not found in word tree!\n", (char *)node->key);
        missing_count++;
    }
    if (rand() & 1) {
        to_delete_count++;
        list_node_t *l = malloc(sizeof(list_node_t));
        if (l != NULL) {
            l->next = delete_list;
            delete_list = l;
            l->key = node->key;
        }
    } else {
        list_node_t *l = malloc(sizeof(list_node_t));
        if (l != NULL) {
            l->next = no_delete_list;
            no_delete_list = l;
            l->key = node->key;
        }
    }
    return(0);
}

static int load_words(void) {
    int fd = open("word.txt", O_RDONLY);
    int rc = 1;
    if (fd == -1) {
        fprintf(stderr, "error opening word list\n");
        goto cleanup;
    }
    int len = lseek(fd, 0, SEEK_END);
    if (len < 0) {
        fprintf(stderr, "error seeking word list\n");
        goto cleanup;
    }
    if (lseek(fd, 0, SEEK_SET) < 0) {
        fprintf(stderr, "error seeking word list\n");
        goto cleanup;
    }
    words = malloc(len);
    if (words == NULL) {
        fprintf(stderr, "error allocating %i bytes for word list\n", len);
        goto cleanup;
    }
    if (read(fd, words, len) != len) {
        fprintf(stderr, "error reading %i bytes from word list\n", len);
        goto cleanup;
    }
    char *p = words;
    char *s = p;
    while (len--) {
        if (*p == '\n') {
            *p = 0;
            if (s[0] != 0) {
                rbtree_node_t *n = rbtree_insert(word_tree, s);
                if (min == NULL) {
                    min = s;
                } else {
                    int cmp = strcmp(min, s);
                    if (cmp > 0) {
                        min = s;
                    }
                }
                if (max == NULL) {
                    max = s;
                } else {
                    int cmp = strcmp(max, s);
                    if (cmp < 0) {
                        max = s;
                    }
                }
                word_count++;
                uint64_t i;
                // make sure we have a unique random number
                while (1) {
                    i = rand();
                    rbtree_node_t *n = rbtree_insert(tmp_tree, (void *)i);
                    if (n->data == NULL) {
                        n->data = s;
                        tmp_count++;
                        break;
                    }
                }
            }
            s = p + 1;
        }
        p++;
    }
    rc = 0;
cleanup:
    close(fd);
    return(rc);
}