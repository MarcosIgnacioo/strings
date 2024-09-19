# Robado de 
https://bytesbeneath.com/p/custom-strings-in-c

  // el end no es que tan largo va a ser algo si no el indice de donde se va a
  // cortar o tcopnsiderar como terminado el string en el caso del viedw bueno
  // creo que en elos otoros casos tambien es asi pero no importa jijijiji



THIS IS SO COOL

Pongamos por ejemplo que se le pone de input este string

    0123456789abcd
    fooasdfasdfa,,,,a,b

La manera en la que este algoritmo/patron de diseño o como se le diga asegura que esto sea splitado de la manera correcta es utilizando el memcmp y el start como un pivot. De manera que, aunque nuestro string para splitear sea una , nos devolveria solamente un arreglo con [fooasdfasdfa, a , b] esto pasa porque cuando llega a las comas el start, por ejemplo en el segundo del bonche de las comas ocupa el indice 13, y pasa a la siguiiente compracaion, checa si el delimitador esta en esa region de la memoria, y compara ambas memorias la que contiene la informacion de como es el delimitador y la de nuestro string que estamos spliteando, por el largo del delimitador, lo que hace que pues se chequen ambcas memorias para ver si el delimitador estas exactamente ahi porque hayt que recordar que delimitador podria ser ,.,/,.m y otras cocsas asi, por lo que si esta presente, en este caso loesta porque en esta region de lam meoria en el indice 13 de nuestro string. i en el contexto del ciclo lo quqe va a contener ees el indice que corresponde la primer delimiter bueno al primer caracc ter del delimiteder que nos hemos encontrado, y start es el indice de donde inicia nuestra palabra, entonces ambos comparten el mismo indice por lo que la length es 0 y pues se retorna un string con length 0 jiji
esta muy chido porque i pues siempre sera la primera vez qeu se encuentre el delimiter, mientras que tenemos a start como un pivot para guardar el indice del primer caracter que vamos a splitear y pues ya nomas usamos a i como final y amonos recio, luego actualizamos start para que tenga el caracter siguiente de nuestro string, el cual se obtiene pues sumando el start antiguo junto a la len del delimiter mas el i porque recordemos que i es el pinche primer caracter  del delimiter que nos enconntramos!!!!!!!

```c
!memcmp(&s.data[i], delimiter.data, delimiter.len)
```
puesto que 
```c
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

```
