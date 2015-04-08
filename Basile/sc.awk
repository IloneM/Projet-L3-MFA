begin{toto = 0; tutu = 0;}
{print cos($1), sin($1), $2, cos($3), sin($3), $4, $2 - toto, $4 - tutu;
  toto = $2;
  tutu = $4;
}
end{}