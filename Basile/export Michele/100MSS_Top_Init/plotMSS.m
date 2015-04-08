data=dlmread("U_trainPlot.txt");
x1 = cos (data(:,1));
y1 = sin (data(:,1));

x2 = x1+cos (data(:,1)+data(:,3));
y2 = y1+sin (data(:,1)+data(:,3));

hold off
close
hold on

axis ([-2 2 -2 2])
j=1;
for i=1:size(x1)(1)

brasx = [0 -y1(i)] ;
brasy = [0 -x1(i)];

jambex = [-y2(i) -y1(i)] ;
jambey = [-x2(i) -x1(i)] ;

if (0== mod(i,100))

hold off
close
hold on

axis ([-2 2 -2 2])
end

plot (brasx,brasy,'-')
plot (jambex ,jambey,'-')

j=j+1;
pause (0.15)
end
hold off

