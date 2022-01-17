

## Pseudokod
Dla *i*=1 do *ilość_iteracji*
	Dla *m*=1 do *ilość_mrówek*
		Dopóki mrówka nie skończy cyklu
			Mrówka wybiera następne miasto *j* do odwiedzenia z prawdopodobieństwem *p_ij* wyliczaną za po mocą wzoru (1)
		Zapisać dystans całego cyklu mrówki, podmienić z dotychczasowym najkrótszym jeśli jest lepszy od niego.
	Zaktualizować poziomy feromonów na ścieżkach za pomocą równań (2-4)
Koniec
$$
p^k_{ij}=\left{ \frac{a}{v} \right}
$$
