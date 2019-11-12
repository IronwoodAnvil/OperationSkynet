[X,Fs] = audioread('..\APT Samples\NOAAAPT_Sound.mp3');

E =  designfilt('lowpassiir', 'DesignMethod', 'ellip', ...
        'PassbandFrequency',2400, 'StopbandFrequency',3000, ...
        'PassbandRipple',0.5,'StopbandAttenuation',20+20*log10(256), ...
        'SampleRate',Fs);
    
forder = filtord(E) % Display filter order
[z,p,k] = zpk(E);
zpk(z,p,k,1/Fs) 

%fvtool(E)
grpdelay(E);

Y = filter(E,abs(X));
Y2 = amdemod(X,2400,Fs); % Compare to built in AM demodulate
t = (0:length(Y))/Fs;

% Compute baud-synchronized samples
oversample = 8;
baud = 4160*oversample;
nsamples = floor(length(Y)*baud/Fs);
% Take most recent sample at each timer tick (oversample*4160)
indicies = floor((1:nsamples)*Fs/baud);
P = Y(indicies);
t2 = (0:length(P))/baud;

% Plot recovered signals
figure(1);
D = grpdelay(E); % Compute DC time delay of filter to compensate to better match
shift = ceil(D(1));
plot(t(1:Fs),Y(shift+1:Fs+shift));
hold on;
plot(t(1:Fs),Y2(1:Fs));
plot(t2(1:baud),P(1:baud));
hold off;

% Generate sync reference signals with correct oversampling
synca = repmat(repmat([1 1 -1 -1],1,7),oversample,1);
synca = synca(:)';
syncb = repmat(repmat([1 1 1 -1.5 -1.5],1,7),oversample,1);
syncb = syncb(:)';

% Find first sync instances
shiftrange = 2400*oversample;
finda = xcorr(P,synca,shiftrange);
finda = finda(shiftrange:end);
[~,shifta] = max(finda)
findb = xcorr(P,syncb,shiftrange);
findb = findb(shiftrange:end);
[~,shiftb] = max(findb)

% plot cross correlation
figure(2);
plot(finda)
hold on;
plot(findb);
hold off;

% plot subsets of baud-synchronized signal to check
figure(3);
stem(P(shifta-50:shifta+50))
figure(4);
stem(P(shiftb-50:shiftb+50))

lines = floor((length(P)-shiftb)/oversample/2080);

image_index_set = reshape(shiftb + (0:oversample:oversample*lines*2080-1),2080,[],1) + reshape([3 4 5 6],[1,1,4]);
image_raw = reshape(sum(P(image_index_set),3),2080,[])';
imax = max(image_raw(:));
imin = min(image_raw(:));
image_raw = (image_raw-imin)/(imax-imin);
figure(5);
imshow(image_raw);