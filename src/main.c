#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  size_t len;
  char *data;
} String;

#define String(str)                                                            \
  (String) { strlen(str), str }

#define temp_allocator()                                                       \
  &(Allocator) { .context = NULL, .alloc = malloc }

typedef struct {
  void *context;
  void *(*alloc)(unsigned long);
} Allocator;

String string_init(size_t len, Allocator *allocator) {
  String new_str = (String){.len = len, .data = allocator->alloc(len + 1)};
  if (!new_str.data)
    return new_str;

  // lo tenemos q hacer asi porque si hago [i] a un null pointer me va a mandar
  // a la verga c
  new_str.data[new_str.len] = 0;
  return new_str;
}

// lo k se hace es inicializar un string nuevo con string_init le ponemos la
// length de la suma de ambos strings y le sumamos uno para el null terminator;
// despues copiamos los bytes de la memoria del primer string al nuevo que
// estamos haciendo y luego copiamoos el del siguiente a partir dla ultima
// posicion del prime string por lo que quedaria pegadiiito
String string_concat(String string_1, String string_2) {
  size_t total_size = string_1.len + string_2.len + 1;
  String new_str_ct = string_init(total_size, temp_allocator());
  char *string = new_str_ct.data;
  memcpy(string, string_1.data, string_1.len);
  memcpy(&string[string_1.len], string_2.data, string_2.len);
  return new_str_ct;
}

// aqui lo que hacemos es basicamente retornar un string vacio en elc aso de que
// el end (hasta donde queremos cortar el string) es mayor a la len del string o
// si el inicio que estamos pasando es mayor al final tiene que ser con
// menormayorigual porque si no
// si el final es mayor a la length del string pues no manches w no podemos
// cortartelo si el inicio es igual o mayor al final pues tampoco, si es igual
// pues seria de longitud 0 por lo que no tiene sentido y pues si es mayor que
// el final no pueede hacerse un substring asiiiii!
String str_substring(String s, size_t start, size_t end, Allocator *allocator) {
  String sub_str = {0};
  if (end > s.len || start >= end)
    return sub_str;
  sub_str = string_init(end - start, allocator);
  memcpy(sub_str.data, &s.data[start], sub_str.len);
  return sub_str;
}
// aqui se hace algo bien inteligente y es usar una variable externa para
// mantener un "cursor" sobre donde se haya quedado la busqueda en el string que
// se bueno se esta buscando para que en el caso de que se tenga un string como
// ppoooopo diga que si que contiene el string popo
// si se quisiera un match exacto se resetearia el cursor j
bool str_contains(String haystack, String needle) {
  bool found = false;
  for (size_t i = 0, j = 0; i < haystack.len && !found; i++) {
    while (haystack.data[i] == needle.data[j]) {
      j += 1;
      i += 1;
      if (j == needle.len) {
        found = true;
        break;
      }
    }
  }
  return found;
}

// match exacto
bool str_match(String haystack, String needle) {
  bool found = false;
  for (size_t i = 0, j = 0; i < haystack.len && !found; i++) {
    while (haystack.data[i] == needle.data[j]) {
      j += 1;
      i += 1;
      if (j == needle.len) {
        found = true;
        break;
      }
    }
    j = 0;
  }
  return found;
}

int string_index_of(String str, char c) {
  size_t i = 0;
  for (i = 0; i < str.len; i++) {
    if (str.data[i] == c)
      break;
  }
  return i;
}

// int string_index_of_string(String haystack, String needle) {
//   size_t index = -1, i = 0, j = 0;
//   for (; i < haystack.len; i++) {
//     j = 0;
//     while (haystack.data[i] == needle.data[j]) {
//       i++;
//       j++;
//       if (j == needle.len)
//         break;
//     }
//   }
//   if (j == needle.len) {
//     return i - needle.len - 1;
//   } else {
//     return -1;
//   }
// }

int string_index_of_string(String haystack, String needle) {
  for (size_t i = 0; i < haystack.len; i++) {
    size_t j = 0;
    size_t index = i;
    while (haystack.data[i] == needle.data[j]) {
      j++;
      i++;
      if (j == needle.len)
        return index;
    }
  }
  return -1;
}

size_t str_index_of(String haystack, String needle) {
  for (size_t i = 0; i < haystack.len; i += 1) {
    size_t j = 0;
    size_t start = i;
    while (haystack.data[i] == needle.data[j]) {
      j += 1;
      i += 1;
      if (j == needle.len) {
        return start;
      }
    }
  }
  return (size_t)-1;
}

// NOTE: this does not terminate the string with a 0 as that would destroy the
/* original string. */

// retorna el substring que se busque aunque pues retorna hasta el final del
// string
String str_substring_view(String haystack, String needle) {
  String r = {0};
  int index_of_needle = str_index_of(haystack, needle);
  if (index_of_needle > haystack.len)
    return r;
  r.data = &haystack.data[index_of_needle];
  r.len = needle.len;
  return r;
}

void print_string(String str) {
  size_t i = 0;
  while (i < str.len) {
    printf("%c", str.data[i++]);
  }
}

void print_string_ln(String str) {
  size_t i = 0;
  while (i < str.len) {
    printf("%c", str.data[i++]);
  }
  printf("\n");
}

bool str_equal(String a, String b) {
  if (a.len > b.len)
    return false;
  size_t i = 0;
  bool are_equal = true;
  while (i < a.len) {
    if (a.data[i] != b.data[i]) {
      are_equal = false;
      break;
    }
    i++;
  }
  return are_equal;
}
bool string_copy(String *to, String from) {
  if (to->len < from.len) {
    return false;
  }
  size_t i = 0;
  while (i < from.len) {
    to->data[i] = from.data[i];
  }
  return true;
}

String str_replace(String s, String match, String replacement, Allocator *a) {
  size_t str_match_idx = str_index_of(s, match);
  size_t new_len = s.len - match.len + replacement.len;
  String new_string = string_init(new_len, a);

  for (size_t i = 0; i < str_match_idx; i++) {
    new_string.data[i] = s.data[i];
  }

  size_t replacement_end = str_match_idx + replacement.len;

  for (size_t i = str_match_idx, j = 0; i < replacement_end; i++) {
    new_string.data[i] = replacement.data[j++];
  }

  for (size_t i = replacement_end, j = str_match_idx + match.len; i < new_len;
       i++) {
    new_string.data[i] = s.data[j++];
  }

  return new_string;
}

String str_view(String s, size_t start, size_t end) {
  if (start <= end)
    return (String){0};

  String string_view = {0};
  // USA PUTISIMA POINTER ARITHMETIC
  string_view.data = s.data + start;
  string_view.len = end - start;

  return string_view;
}

// aqui podria no usar string init para que el callstack no se haga un cagadero
// pero k weba clonned_string.data = a.alloc(s.len + 1) clonned_string.len =
// s.len
String str_clone(String s, Allocator *a) {
  String clonned_string = string_init(s.len, a);
  if (!s.len)
    return (String){0};
  memcpy(clonned_string.data, s.data, s.len);
  return clonned_string;
}

String *str_split(String s, String delimiter, Allocator *a) {
  String buffer = string_init(s.len, a);
  for (size_t i = 0, j = 0; i < s.len; i++) {
    j = 0;
    while (s.data[i] == delimiter.data[j]) {
      if (j == delimiter.len - 1) {
      }
    }
  }
}

int main(void) {
  Allocator a = {.context = NULL, .alloc = malloc};
  String test = String("asdfasdfsdfpopojasdfasdfjajaaj");
  String test3 = String("popo");
  String test2 = String("animal crossing");
  String stringc = string_concat(test, test2);
  String sub = str_substring(test, 2, 7, &a);
  String found = str_substring_view(test, test3);
  String replaced = str_replace(test, test3, test2, &a);
  String clonned = str_clone(test, &a);
  print_string_ln(test);
  print_string_ln(clonned);
}
