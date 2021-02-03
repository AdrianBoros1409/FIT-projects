% ISS projekt
% Adrian Boros [xboros03]

%1.)
[signal, Fs]=audioread('xboros03.wav');
signal=signal';
N=length(signal);
T=N/Fs;
s=N/16;
subor=importdata('xboros03.txt');
subor=subor';
 
%2.)
result=zeros(1,s);
j=1;
i=8;
while i < N
   if signal(i) > 0
       result(j)=1;
       j=j+1;
   else
       result(j)=0;
       j=j+1;
   end
   i=i+16;
end
x = signal(1:0.02 * Fs);
t=[1:320]/Fs;
hold on
stem(0.02/(20*2):0.02/20:0.02,result(1:20));
plot(t,x);
title('Dekódovanie s[n] do binárnych symbolov');
xlabel('t');
ylabel('s[n],symbols');
grid;
hold off
figure(2);
porovnaj_subor=xor(result,subor);
i=1;
counter=0;
while i < length(porovnaj_subor)
   if subor(i) ~= result(i)
       counter=counter+1;
       i=i+1;
   else
       i=i+1;
   end
end

%3.)
B = [0.0192 -0.0185 -0.0185 0.0192];
A = [1.0000 -2.8870 2.7997 -0.9113];
zplane(B,A);
stable=isstable(B,A);
title('Nulové body a póly');
figure(3);

%4.)
H = freqz(B,A,256); f=(0:255) / 256 * Fs / 2; 
plot(f,abs(H)); grid; xlabel('f [Hz]'); ylabel('|H(f)|');
title('Modul kmitoètovej charakteristiky');
[M,I]=min(H(1:30));
hold on
stem(f(I), abs(M));
hold off
figure(4);
 
%5.)
ss=filter(B,A,signal);
x = ss(1:0.02 * Fs);
t=[1:320]/Fs;
hold on
plot(t,x);
x = signal(1:0.02 * Fs);
plot(t,x);
title('Vyfiltrovaný signál'); 
xlabel('t'); 
ylabel('s[n],symbols');
grid;
hold off
figure(5);
  
%6.)
posun=-16;
shifted=circshift(ss,posun);
x=shifted(1:0.02 * Fs);
t=[1:320]/Fs;
hold on
plot(t,x);
x = signal(1:0.02 * Fs);
plot(t,x);
x = ss(1:0.02 * Fs);
plot(t,x);
stem(0.02/(20*2):0.02/20:0.02,result(1:20));
title('Posunutý signál'); xlabel('t'); ylabel('s[n],ss[n],ss_{shifted}[n],symbols');
hold off
figure(6);
binary=zeros(1,s);
j=1;
i=8;
while i < N
    if shifted(i) > 0
        binary(j)=1;
        j=j+1;
    else
        binary(j)=0;
        j=j+1;
    end
    i=i+16;
end

%7.)
porovnanie=xor(binary,result);
i=1;
pocitadlo=0;
while i < length(porovnanie)
   if binary(i) ~= result(i)
       pocitadlo=pocitadlo+1;
       i=i+1;
   else
       i=i+1;
   end
end
chybovost=pocitadlo/s*100;

%8.)
signal_spectr=fft(signal);
hold on
ss_spectr=fft(ss);
title('Spektrá signálov');
plot(abs(signal_spectr(1:Fs/2)));
plot(abs(ss_spectr(1:Fs/2)));
xlabel('f [Hz]'); 
ylabel ('PSD [dB]'); 
grid;
hold off
figure(7);
 
%9.)
osa_max=max(signal);
osa_min=min(signal);
kolik=100;
g=linspace(osa_min,osa_max,kolik);
deltag=g(2)-g(1);
pg=hist(signal,g)/N/deltag;
plot(g,pg);
title('Odhad funkcie hustoty rozdelenia pravdepodobnosti');
figure(8);
vysledok=sum(pg*deltag);


%10.)
k=(-50:50);
Rk=xcorr(signal,'biased');
Rk=Rk(k+N);
plot(k, Rk); xlabel('k'); ylabel('R[k]');
title('Hodnoty korelaèných koeficientov');
figure(9);
 
% %11.)
Rk0 = Rk(51);
Rk1 = Rk(52);
Rk16 = Rk(67);

%12.) 
N = length(signal);
L = 50;
h = zeros(L, L);
x = linspace(min(signal), max(signal), 50);
xcol = x(:);
bigx = repmat(xcol,1,N); 
yr=signal(:)';
bigy = repmat(yr,L,1);
[dummy, ind1] = min(abs(bigy - bigx));
ind2 = ind1(1 + 1 : N);
for ii=1:N-1
   d1 = ind1(ii);   d2 = ind2(ii); 
   h(d1,d2) = h(d1,d2) + 1; 
end
surf = (x(2) - x(1)) ^ 2;
p = h / N / surf;
imagesc(x, x, p);
axis xy;
colorbar;
xlabel('x_2');
ylabel('x_1');
title('Èasový odhad združenej funkcie hustoty rozdelenia pravdepodobnosti');

 
%13.)
check = sum(sum (p)) * surf;  
fprintf('hist2: check -- 2d integral should be 1 and is %f \n',check);

%14.)
%%%% autocor coeff 
% make col vector out of x and clone it L times. 
x = x(:); X1 = repmat(x,1,L);
% make row vector out of x and clone it L times. 
x = x'; X2 = repmat(x,L,1); 
% now put it together, don't forget to multipl by tile surface
r = sum(sum (X1 .* X2 .* p)) * surf;
