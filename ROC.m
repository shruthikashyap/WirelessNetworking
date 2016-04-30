Prob_noSig = makedist('Normal', 'mu', -69.63, 'sigma', 2.72);
Prob_Sig = makedist('Normal', 'mu', -50.83, 'sigma', 5.4);
threshold = -60;
pfa = 1 - cdf(Prob_noSig, threshold)
pd = 1 - cdf(Prob_Sig, threshold)
level = [-90:-30];

figure(1);
plot(level, Prob_noSig.pdf(level));
hold on
plot(level,Prob_Sig.pdf(level), 'm');
title('RTL-SDR detection probability')
hold on
Y = 0:0.1:0.3;
X = threshold * ones(size(Y))
plot(X, Y, 'r--');
legend('No target present', 'Target present', 'Threshold')
xlabel ('level (dB)');

pfa_ROC = 1 - cdf(Prob_noSig, level);
pd_ROC = 1 - cdf(Prob_Sig, level);
		
figure(2);
plot(pfa_ROC, pd_ROC);
		
title('ROC curve');
ylabel ('Probability of Detection');
xlabel ('Probability of False Alarm');
