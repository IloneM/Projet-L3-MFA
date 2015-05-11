function res=coutI(d) 
    res = 1000 .* d +1/6 .* (2 .* d +1) .* (d+1) .* d;
endfunction

function res=cout(d) 
    res = coutI(d) - coutI(32);
endfunction

i=32;

while cout(i) < cout(37) / 5
    i = i+1;
end

disp(i)

disp(cout(i-1))

disp(cout(i))

while cout(i) < 3 .* cout(37) ./ 5
    i = i+1;
end

disp(i)

disp(cout(i-1))

disp(cout(i))
