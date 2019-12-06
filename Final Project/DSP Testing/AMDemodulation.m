[X,Fs] = audioread('..\APT Samples\Full Image.ogg');
Fs
if Fs == 44100
X = resample(X,1664,2205);
elseif Fs == 48000
X = resample(X,52,75);
X = X((2080*8*100):end);
end
oversample = 8;
Fs = 4160*oversample;

E =  designfilt('lowpassiir', 'DesignMethod', 'ellip', ...
        'PassbandFrequency',2100, 'StopbandFrequency',2700, ...
        'PassbandRipple',0.5,'StopbandAttenuation',20+20*log10(256), ...
        'SampleRate',Fs);

    
%fvtool(E);
%grpdelay(E);

forder = filtord(E) % Display filter order
[nc,dc] = tf(E);
nc'
-dc'
%[z,p,k] = zpk(E);
%zpk(z,p,k,1/Fs)


Y = filter(E,abs(X));
Y2 = amdemod(X,2400,Fs); % Compare to built in AM demodulate
t = (0:length(Y))/Fs;

% Plot recovered signals
figure(1);
D = grpdelay(E); % Compute DC time delay of filter to compensate to better match
shift = ceil(D(1));
plot(t(1:Fs),Y(shift+1:Fs+shift));
hold on;
plot(t(1:Fs),Y2(1:Fs));
hold off;

% Generate sync reference signals with correct oversampling
synca = repmat(repmat([1 1 -1 -1],1,7),oversample,1);
synca = synca(:)';
syncb = repmat(repmat([1 1 1 -1.5 -1.5],1,7),oversample,1);
syncb = syncb(:)';

% Find first sync instances
shiftrange = 2400*oversample;
finda = xcorr(Y,synca,shiftrange);
finda = finda(shiftrange:end);
[~,shifta] = max(finda);
findb = xcorr(Y,syncb,shiftrange);
findb = findb(shiftrange:end);
[~,shiftb] = max(findb);

% plot cross correlation
figure(2);
plot(finda)
hold on;
plot(findb);
hold off;

% plot subsets of baud-synchronized signal to check
figure(3);
stem(Y(shifta-50:shifta+50))
figure(4);
stem(Y(shiftb-50:shiftb+50))

lines = floor((length(Y)-shifta)/oversample/2080);

image_index_set = reshape(shifta + (0:oversample:oversample*lines*2080-1),2080,[],1) + reshape([3 4 5 6],[1,1,4]);
img = reshape(sum(Y(image_index_set),3),2080,[])';
imax = max(img(:));
imin = min(img(:));
img = (img-imin)/(imax-imin);
img = floor(img*256)/256;
figure(5);
imshow(img);
imwrite(img,'Full_Image.png');