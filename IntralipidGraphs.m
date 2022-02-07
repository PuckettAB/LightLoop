%FOR THOSE WITH TWO TRIALS
%edit file names in place of the first parameter to match the files you'd like to run
%baseline
X1 = readmatrix('Constant.csv', 'Range', 'A2:A552');
Y1 = readmatrix('Constant.csv', 'Range', 'B2:B552');

%trial 1
X2 = readmatrix('Test1.csv', 'Range', 'A2:A552');
Y2 = readmatrix('Test1.csv', 'Range', 'B2:B552');

%trial 2
X4 = readmatrix('Test2.csv', 'Range', 'A2:A552');
Y4 = readmatrix('Test2.csv', 'Range', 'B2:B552');

plot(X1, Y1,'DisplayName','Baseline'), grid on
hold on
plot(X2,Y2,'DisplayName','Test 1')
plot(X4,Y4,'DisplayName','Test 2')
 
title('Constant vs. 90% Intralipid 10% Water (with ink) in 100ml beaker');
ylabel('Absorbance');
xlabel('Wavelength (nm)');
axis([650 900 0 10]);
lgd = legend;
lgd.NumColumns = 2;
hold off
 
figure();
plot(X2,Y2,'DisplayName','Test 1')
hold on
plot(X4,Y4,'DisplayName','Test 2')
 
title('840nm to 880nm -- 90% Intralipid 10% Water (with ink) in 100ml beaker');
ylabel('Absorbance');
xlabel('Wavelength (nm)');
axis([800 860 0 10.5]);
lgd = legend;
lgd.NumColumns = 2;
hold off
 
 
% Use hold on command to plot on same graph
 
 
 
Matlab code
%FOR THOSE WITH 1 TRIALS
%edit file names in place of the first parameter to match the files you'd like to run
%baseline
X1 = readmatrix('Constant.csv', 'Range', 'A2:A552');
Y1 = readmatrix('Constant.csv', 'Range', 'B2:B552');

%trial 1
X2 = readmatrix('Test1.csv', 'Range', 'A2:A552');
Y2 = readmatrix('Test1.csv', 'Range', 'B2:B552');


plot(X1, Y1,'DisplayName','Baseline'), grid on
hold on
plot(X2,Y2,'DisplayName','Test 1')
 
title('Constant vs. Intralipid (with ink) in 100ml beaker');
ylabel('Absorbance');
xlabel('Wavelength (nm)');
axis([650 1200 -1.5 10.2]);
lgd = legend;
lgd.NumColumns = 2;
hold off
 
 
figure();
plot(X2,Y2,'DisplayName','Test 1')
 
title('840nm to 880nm -- Water in 100ml Beaker, 50% Intralipid, 50% water in 100ml beaker');
ylabel('Absorbance');
xlabel('Wavelength (nm)');
axis([800 860 0 10.5]);
lgd = legend;
lgd.NumColumns = 2;
hold off
 
 
% Use hold on command to plot on same graph
 
 
 
 
 
 
 
 
%percent change
 
%baseline
X1 = readmatrix('Constant.csv', 'Range', 'A2:A552');
Y1 = readmatrix('Constant.csv', 'Range', 'B2:B552');

%Comparison 1
X2 = readmatrix('Comp1.csv', 'Range', 'A2:A552');
Y2 = readmatrix('Comp1.csv', 'Range', 'B2:B552');
%Comparison 2
X2 = readmatrix('Comp2.csv', 'Range', 'A2:A552');
Y2 = readmatrix('Comp2.csv', 'Range', 'B2:B552');
%Comparison 3
X2 = readmatrix('Comp3.csv', 'Range', 'A2:A552');
Y2 = readmatrix('Comp3.csv', 'Range', 'B2:B552');
%Comparison 4
X2 = readmatrix('Comp4.csv', 'Range', 'A2:A552');
Y2 = readmatrix('Comp4.csv', 'Range', 'B2:B552');
 
 
Y3 = ((Y2-Y1)./Y1)*100;
X3 = ((X2-X1)./X1)*100;
 
plot(X1, Y1,'DisplayName','Baseline'), grid on
hold on
%plot(X2,Y2,'DisplayName','Test')
plot(X1,Y3,'DisplayName','Percent Change')
 
title('Constant vs. Intralipid ');
ylabel('Absorbance');
xlabel('Wavelength (nm)');
axis([650 1200 -1.5 10.2]);
lgd = legend;
lgd.NumColumns = 2;
hold off
 
 
 
