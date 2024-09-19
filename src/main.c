#include "./array_list.c"
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
  size_t len;
  char *data;
} String;

#define String(str)                                                            \
  (String) { strlen(str), str }

#define ARRAY_CAPACITY 512
#define array(T, a) array_init(sizeof(T), ARRAY_CAPACITY, a)
#define array_header(array) (((Array_Header *)array) - 1)
#define array_length(array) ((array_header(array))->length)
#define array_capacity(array) ((Array_Header *)(array)->capacity)

#define array_append(a, v)                                                     \
  ((a) = array_ensure_capacity(a, 1, sizeof(*a)),                              \
   (a)[array_header(a)->length] = (v), &(a)[array_header(a)->length++])

#define array_remove(array, idx)                                               \
  do {                                                                         \
    Array_Header *header = array_header(array);                                \
    int last_idx = header->length - 1;                                         \
    if (last_idx == idx) {                                                     \
      header->length--;                                                        \
    } else if (header->length > 1) {                                           \
      void *free_space = &array[idx];                                          \
      void *last = &array[last_idx];                                           \
      memcpy(free_space, last, sizeof(*array));                                \
      header->length--;                                                        \
    }                                                                          \
  } while (0);

#define pop_back(array) ((array)[array_header(array)->length])

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
String string_concat(String string_1, String string_2, Allocator *a) {
  size_t total_size = string_1.len + string_2.len + 1;
  String new_str_ct = string_init(total_size, a);
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
  // XD
  //
  if (end - start > s.len)
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

// esta manera esta chida porque funciona de la siguiente manera
// cuando llegamos aqui sabemos que estamos justo dentro de un delimitador
// entonces si por ejemplo, nuestro string de entrada fuese el siguiente
// poopDELIMasdfDELIMgjk
// la primera vez que entrariamos aqui i valdria 4 (recordemos que hacemos
// comparacion con la memoria y no vamos viendo caracter por caracter como
// en otras funciones) entonces, tenemos una variable start que al inicio
// pues es 0, entonces, sabemos que i es donde se termina el string por lo
// que tenemos una variable de inicio del string (start 0) y del final (i
// 4), hacemos un substring con esos dos numeros, y actualizamos el inicio
// de una manera super cool y esa es sumandole a nuestro final (i 4) el
// tamaño del propio delimitador, en este caso le sumariamos 5, y ahora
// nuestro start estaria en 9 que justo pues le corresponde al siguiente
// elemento, y creo que si hacemos que i valga start podemos ahorrarnos
// unas iteraciones puesto que no recorreriamos y pues nos saltamos el if
// del inicio pero pues luego vemos
// TODO impllemnetar lo k dije aqwui arriba mije

String *str_split(String s, String delimiter, Allocator *a) {
  String *arr = array(String, a);
  size_t i = 0, segment_length = 0;
  size_t start = 0;
  for (; i < s.len; i++) {
    if (s.data[i] != delimiter.data[0]) {
      segment_length++;
      continue;
    }
    if (!memcmp(&s.data[i], delimiter.data, delimiter.len)) {
      segment_length = 0;
      size_t end = i;
      String cloned = str_substring(s, start, end, a);
      if (cloned.len)
        array_append(arr, cloned);
      start = end + delimiter.len;
      i = start;
    }
  }

  if (start < s.len) {
    // en este punto start tiene el indice del ultimo caracter que se quedo
    // a,b,c
    // en este caso se habria quedaado en c (4)
    // por lo que hacemos un substring desde ese indice del arreglo original y
    // hasta el final del arreglo porque pues esto es lo ulitmmo, solamente se
    // entra aqui si start es menor a s.len, eso quiere decir que en el caso de
    // que haya un ,a nuestro ciclo no lo agarraria porque se encontraria con el
    // separador y luego intentaria hacer un substring desde 0 hasta el final
    // que seria 0, por lo que no estaria haciendo un substring de nada, por lo
    // que, no lo pone el ciclo, y start estaria en 1 no se XD todo? explicacion
    array_append(arr, str_substring(s, start, s.len, a));
  }

  return arr;
}

String *str_split_view(String s, String delimiter, Allocator *a) {
  String *splitted_view_array = array(String, a);
  size_t start = 0;
  for (size_t i = 0; i < s.len; i++) {
    if (s.data[i] != delimiter.data[0]) {
      continue;
    }
    if (!memcmp(&s.data[i], delimiter.data, delimiter.len)) {
      String splitted_str = {0};
      splitted_str = str_view(s, start, i);
      if (splitted_str.len)
        array_append(splitted_view_array, splitted_str);
      start = i + delimiter.len;
    }
  }

  if (start < s.len) {
    String splitted_str = {0};
    splitted_str = str_view(s, start, s.len);
    if (splitted_str.len)
      array_append(splitted_view_array, splitted_str);
  }
  return splitted_view_array;
}

String str_join(String *s, String join, Allocator *a) {
  String joined_str = (String){0};
  size_t arr_len = array_length(s);
  size_t start = 0;
  for (size_t i = 0; i < arr_len; i++) {
    if (i > 0) {
      joined_str = string_concat(joined_str, join, a);
    }
    joined_str = string_concat(joined_str, s[i], a);
  }
  return joined_str;
}

String str_join_faster(String *s, String join, Allocator *a) {
  Array_Header *h = array_header(s);
  String joined_str = (String){0};
  size_t offset = 0, total_size = 0;
  for (size_t i = 0; i < h->length; i++) {
    total_size += s[i].len;
  }
  // a,df,s,f,a
  total_size += join.len * h->length;
  joined_str.data = a->alloc(total_size);
  joined_str.len = total_size;

  for (size_t i = 0; i < total_size; i++) {
    memcpy(&joined_str.data[offset], s[i].data, s[i].len);
    offset += s[i].len;
    if (offset == total_size) {
      break;
    }
    memcpy(&joined_str.data[offset], &join.data[i], join.len);
    offset += join.len;
  }
  joined_str.data[offset - 1] = 0;
  return joined_str;
}

int main(void) {
  Allocator a = {.alloc = my_alloc, .free = my_free, .context = NULL};
  String *arr = array(String, &a);
  // String test = String("asdfasdfsdfpopojasdfasdfjajaaj");
  String split_test = String("fooasdfasdfa,,,,a,b");
  String *arrgamer = str_split_view(split_test, String(","), &a);
  String j = str_join_faster(arrgamer, String("/"), &a);
  print_string_ln(j);

  // for (size_t i = 0; i < array_length(arrgamer); i++) {
  //   print_string_ln(arrgamer[i]);
  // }
  return 0;
  // void *adsf = NULL;
  // // &(arr)[array_header(arr)->length++]);
  // String test3 = String("popo");
  // String test2 = String("animal crossing");
  // String splitr = String("hola,amigos,como,estan");
  // String stringc = string_concat(test, test2);
  // String sub = str_substring(test, 2, 7, &a);
  // String found = str_substring_view(test, test3);
  // String replaced = str_replace(test, test3, test2, &a);
  // String clonned = str_clone(test, &a);
  // print_string_ln(test);
  // print_string_ln(clonned);
  // str_split(split_test, String("pp"), &a);
}
