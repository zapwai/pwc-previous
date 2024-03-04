#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>

#define max_length 50
#define thou 1000
#define rows_in_file 50000	/* using a custom word file w 36k words */
#define limit 4			/* max chain length is twice limit */

char** read_file(char* filename, int* num_words) {
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) return NULL;
  char** words = malloc(rows_in_file * sizeof(char*));
  if (words == NULL) return NULL;
  char buffer[max_length];
  int i = 0;
  while (fgets(buffer, max_length, fp) != NULL) {
    words[i] = malloc(max_length * sizeof(char));
    if (words[i] == NULL) return NULL;
    strcpy(words[i], buffer);
    i++;
  }
  fclose(fp);
  *num_words = i;
  return words;
}

void chomp(char** words) {
  for (int i = 0; words[i] != NULL; i++) {
    char* str = words[i];
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
      str[len - 1] = '\0';
  }
}

char** unique(char** words) {
  char** subset = malloc(thou * sizeof(char*));
  for (int i = 0; i < thou; i++) { subset[i] = NULL; }
  int k = 0;
  for (int i = 0; words[i] != NULL; i++) {
    bool flag = false;
    for (int j = 0; subset[j] != NULL; j++)
      if (strcmp(words[i], subset[j]) == 0) {
	flag = true;
	break;
      }
    if (!flag) {
      subset[k] = malloc((1+strlen(words[i])) * sizeof(char));
      strcpy(subset[k], words[i]);
      k++;
    }
  }
  return subset;
}

char** intersection(char** list1, char** list2) {
  char** common = malloc(thou * sizeof(char*));
  for (int i = 0; i < thou; i++)
    common[i] = malloc(max_length * sizeof(char));
  int k = 0;
  for (int i = 0; list1[i] != NULL; i++) {
    for (int j = 0; list2[j] != NULL; j++) {
      if (0 == strcmp(list1[i], list2[j])) {
	strcpy(common[k], list1[i]);
	k++;
      }
    }
  }
  return common;
}

char** check(int m, int n, char*** A, char*** B) {
  char** ans = malloc(thou * sizeof(char*));
  for (int i = 0; i < thou; i++)
    ans[i] = malloc(max_length * sizeof(char));
  int k = 0;
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < m; j++) {
      char** common = intersection(A[i], B[j]);
      for (int l = 0; common[l] != NULL; l++) {
	strcpy(ans[k], common[l]);
	k++;
      }
      for (int l = 0; l < thou; l++) 
	free(common[l]);
      free(common);
    }
  }
  return ans;
}

char** expand(char* word, char** words, int* length) {
  char** new = malloc(thou * sizeof(char*));
  for (int i = 0; i < thou; i++)
    new[i] = malloc(max_length * sizeof(char));
  int k = 0;
  for (int i = 0; i < strlen(word); i++) {  
    char pre[max_length] = {};
    char post[max_length] = {};
    strncpy(pre, word, i);
    strncpy(post, word + i + 1, strlen(word) - i);
    char rexp[max_length] = {};
    strcat(rexp, pre);
    strcat(rexp, ".");
    strcat(rexp, post);
    regex_t regex;
    int ret = regcomp(&regex, rexp, REG_EXTENDED);
    if (ret != 0){
      fprintf(stderr, "failure to compile the regex.\n");
      return NULL;
    }
    for (int j = 0; words[j] != NULL; j++) {
      ret = regexec(&regex, words[j], 0, NULL, 0);
      if (ret == 0) {
	if (0 != strcmp(words[j], word)) {
	  strcpy(new[k],words[j]);
	  k++;
	}
      }
    }
    regfree(&regex);
  }
  *length = k;
  return new;
}

int dist(char* word1, char* word2) {
  int cnt = 0;
  for (int i = 0; i < strlen(word1); i++)
    if (word1[i] != word2[i])
      cnt++;
  return cnt;
}

char* neighbor(char* word, char** word_list) {
  for (int i = 0; word_list[i] != NULL; i++)
    if (1 == dist(word, word_list[i]))
      return word_list[i];
  return NULL;
}

char** grep_for_length(size_t length, char** dict) {
  char** words = malloc(rows_in_file*sizeof(char*));
  for (int i = 0; i < rows_in_file; i++)
    words[i] = malloc(max_length*sizeof(char));
  int k = 0;
  for (int i = 0; dict[i] != NULL; i++) {
    if (strlen(dict[i]) == length){
      strcpy(words[k],dict[i]);
      k++;
    }
  }
  return words;
}

void proc(char** dict, char* input1, char* input2) {
  printf("Input: %s to %s\n", input1, input2);

  int wordlen = strlen(input1);
  if (wordlen != strlen(input2)) {
    printf("Lengths are not equal.\n");
    return;
  }
  int cnt = 0;
  for (int i = 0; dict[i] != NULL; i++) {
    if ((0 == strcmp(input1, dict[i])) || (0 == strcmp(input2, dict[i])))
      cnt++;
  }
  if (cnt < 2) {
    printf("One of these inputs is not considered a word.\n");
    return;
  }

  char** words = grep_for_length(wordlen, dict);

  char*** A = malloc(limit*sizeof(char**));
  char*** B = malloc(limit*sizeof(char**));
  for (int i = 0; i < limit; i++) {
    A[i] = malloc(thou * sizeof(char*));
    B[i] = malloc(thou * sizeof(char*));
    for (int j = 0; j < thou; j++) {
      A[i][j] = malloc(max_length * sizeof(char));
      B[i][j] = malloc(max_length * sizeof(char));
    }
  }

  strcpy(A[0][0], input1);
  strcpy(B[0][0], input2);

  int Acnt[limit] = {};		/* these record the number of cells at each level of A or B*/
  int Bcnt[limit] = {};		/* corrects a segfault from using A[lvl][i]!=null to access A*/
  Acnt[0] = 1;
  Bcnt[0] = 1;
  
  int lvl = 0;

  char** center = malloc(thou * sizeof(char*));
  for (int i = 0; i < thou; i++)
    center[i] = malloc(max_length * sizeof(char));
  
  do {
    char** anew = malloc(thou *sizeof(char*));
    char** bnew = malloc(thou *sizeof(char*));
    for (int i = 0; i < thou; i++) {
      anew[i] = malloc(max_length * sizeof(char));
      bnew[i] = malloc(max_length * sizeof(char));
    }
    int k = 0;
    for (int i = 0; i < Acnt[lvl]; i++) {
      int temp_length;
      char** temp = expand(A[lvl][i], words, &temp_length);
      for (int j = 0; j < temp_length; j++) {
	strcpy(anew[k], temp[j]);
	k++;
      }
      for (int l = 0; l < thou; l++)
	free(temp[l]);
      free(temp);
    }
    k = 0;
    for (int i = 0; i < Bcnt[lvl]; i++) {
      int temp_length;
      char** temp = expand(B[lvl][i], words, &temp_length);
      for (int j = 0; j < temp_length; j++) {
	strcpy(bnew[k], temp[j]);
	k++;
      }
      for (int l = 0; l < thou; l++)
	free(temp[l]);
      free(temp);
    }

    char** a_uniq = unique(anew);
    char** b_uniq = unique(bnew);

    /* When you push to A and B, update Acnt and Bcnt. */
    /* push A, a_uniq */
    /* center = check(lvl + 1, lvl, A, B);  or not...*/
    /* push B, b_uniq */
    
    for (int i = 0; i < thou; i++) {
      free(a_uniq[i]);
      free(b_uniq[i]);
      free(anew[i]);
      free(bnew[i]);
    }
    free(a_uniq);
    free(b_uniq);
    free(anew);
    free(bnew);
    lvl++;
    /* center = check(lvl, lvl, A, B); */
    /* if (center is non empty) {break} */
  } while (lvl < limit);

  if (lvl == limit) {
    printf("Output: ()\n");
    return;
  }

  
  /* more here, to find the chain */
  
  for (int i = 0; i < limit; i++) {
    for (int j = 0; j < thou; j++) {
      free(A[i][j]);
      free(B[i][j]);
    }
    free(A[i]);
    free(B[i]);
  }
  free(A);
  free(B);
  for (int i = 0; i < thou; i++)
    free(center[i]);
  free(center);
}

int main(int argc, char* argv[]) {
  char** words;
  char* filename = "words";
  int words_len;
  words = read_file(filename, &words_len);
  chomp(words);

  char* input1 = "cold";
  char* input2 = "warm";
  proc(words, input1, input2);

  /* char* l1[] = {"pour", "cold", "peer", "knife", "prince"}; */
  /* char* l2[] = {"made", "warm", "norm", "dance", "prance"}; */
  /* for (int i = 0; i < sizeof(l1)/sizeof(char*); i++) */
  /*   proc(words, l1[i], l2[i]); */

  for (int i = 0; i < words_len; i++)
    free(words[i]);
  free(words);
  return 0;
}

