pkg load statistics
data=dlmread("U_trainPlot.txt");
x1 = cos (data(:,1));
y1 = sin (data(:,1));

x2 = cos (data(:,3));
y2 = sin (data(:,3));

x3 = data(:,2);
x4 = data(:,4);
 x = [[x1] [ y1] [ x2] [ y2] [ x3]  [x4]];
[vrac, centers] = kmeans(x(1:8000,:), 100);

file_id = fopen('centers.txt', 'w')
fdisp(file_id, centers)
fclose(file_id)

