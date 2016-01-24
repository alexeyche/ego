

%X =  dlmread('~/rosen.csv', ',');
%Y =  dlmread('~/rosenY.csv', ',');

data = dlmread('/var/tmp/testfuncs.csv');

X = data(:,1:end-1);
Y = data(:,end);

%Y=(Y-mean(Y))/cov(Y);
Xtr = X;
Ytr = Y;
[n, D] = size(X);




%meanF = {@meanSum, {@meanLinear, @meanConst}}; hyp.mean = ones(D+1, 1);
%meanF = {@meanLinear}; hyp.mean = ones(D, 1);
meanF = {@meanConst}; hyp.mean = 0.0;
%covF = {@covMaternard, 5}; 
%covF = {@covGaborard}; hyp.cov = log(ones(2*D,1));
%covF = {@covMaterniso, 5}; hyp.cov = [1.0 1.0];
%covF = {@covMaternard, 1}; hyp.cov = ones(D+1, 1);
covF = {@covSEiso}; hyp.cov = log([1.0 1.0]);
%covF = {@covSEard}; hyp.cov = ones(D+1,1);

%covF = {@covSum, {{@covMaternard, 1}, {@covRQard}}}; hyp.cov = log(ones((D+1)*2 + 1, 1));
%covF = {@covLINard}; hyp.cov = log(ones(D, 1));
%covF = {@covRQard}; hyp.cov = log(ones(D+2, 1));
%covF = {@covRQiso}; hyp.cov = log(ones(3, 1))
%covF = {@covPPard}; hyp.cov = log(ones(D+1,1));

lik = {@likGauss}; hyp.lik = log(0.01);
%lik = {@likT}; hyp.lik = log([0.1 0.1]);
%lik = {@likGamma, 'logistic'}; Ytr = abs(Ytr); Yte = abs(Yte); hyp.lik = log(0.1);
%lik = {@likExp, 'logistic'}; Ytr = abs(Ytr); Yte = abs(Yte); hyp.lik = [];
%lik = {@likInvGauss, 'exp'}; Ytr = abs(Ytr); Yte = abs(Yte); hyp.lik = [log(1.1)];
%lik = {@likGaussWarp, 'poly3'}; hyp.lik=log([1.0 1.0 1.0]); % bad
%lik = {@likGumbel, '-'}; hyp.lik=log(1.0);

inf = {@infExact};                          % inference method
%inf = {@infLaplace};
%inf = {@infEP};
%inf = {@infMCMC};
%m = feval(meanF{:}, hyp.mean, Xtr);
%K = feval(covF{:}, hyp.cov, Xtr);

[post nlZ dnlZ] = feval(inf{:}, hyp, meanF, covF, lik, Xtr, Ytr);


hyp = minimize(hyp,'gp', -100, inf, meanF, covF, lik, Xtr, Ytr); % opt hypers

%Xte = [0.5, 0.6]'; %[0.5 0.5; 0.166667 0.5];

Xte = linspace(0,1, 10)';

[yte_mu, yte_s2] = gp(hyp, inf, meanF, covF, lik, Xtr, Ytr, Xte);  % predict
figure(2); hold on;
plot(X, Y, 'd');
plot(Xte, yte_mu);
plot(Xte, yte_mu - sqrt(yte_s2), '-r');
plot(Xte, yte_mu + sqrt(yte_s2), '-r');
