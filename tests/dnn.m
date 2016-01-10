

data =  dlmread('~/mc.csv', ' ');
X = data(:, 1:end-1);
Y = data(:, end);
Y = log(abs(Y));
Y=(Y-mean(Y))/cov(Y);

Xtr = X(1:800,:);
Ytr = Y(1:800);

Xte = X(801:end,:);
Yte = Y(801:end);
[n, D] = size(X);

%meanF = {@meanSum, {@meanLinear, @meanConst}}; hyp.mean = ones(D+1, 1);
%meanF = {@meanLinear}; hyp.mean = ones(D, 1);
%meanF = {@meanConst}; hyp.mean = 1.0;
%covF = {@covMaternard, 5}; 
%covF = {@covGaborard}; hyp.cov = log(ones(2*D,1));
%covF = {@covMaterniso, 5}; hyp.cov = [1.0 1.0];
%covF = {@covMaternard, 1}; hyp.cov = ones(D+1, 1);
%covF = {@covSEiso}; hyp.cov = log([1.0 1.0]);
%covF = {@covSEard}; hyp.cov = ones(D+1,1);

%covF = {@covSum, {{@covMaternard, 1}, {@covRQard}}}; hyp.cov = log(ones((D+1)*2 + 1, 1));
%covF = {@covLINard}; hyp.cov = log(ones(D, 1));
%covF = {@covRQard}; hyp.cov = log(ones(D+2, 1));
%covF = {@covRQiso}; hyp.cov = log(ones(3, 1))
%covF = {@covPPard}; hyp.cov = log(ones(D+1,1));

lik = {@likGauss}; hyp.lik =  log(1.0);
%lik = {@likT}; hyp.lik = log([0.1 0.1]);
%lik = {@likGamma, 'logistic'}; Ytr = abs(Ytr); Yte = abs(Yte); hyp.lik = log(0.1);
%lik = {@likExp, 'logistic'}; Ytr = abs(Ytr); Yte = abs(Yte); hyp.lik = [];
%lik = {@likInvGauss, 'exp'}; Ytr = abs(Ytr); Yte = abs(Yte); hyp.lik = [log(1.1)];
%lik = {@likGaussWarp, 'poly3'}; hyp.lik=log([1.0 1.0 1.0]); % bad
%lik = {@likGumbel, '-'}; hyp.lik=log(1.0);

inf = {@infExact};                                  % inference method
%inf = {@infLaplace};
%inf = {@infEP};
%inf = {@infMCMC};

hyp = minimize(hyp,'gp', -30, inf, meanF, covF, lik, Xtr, Ytr); % opt hypers


[yte_mu, yte_s2] = gp(hyp, inf, meanF, covF, lik, Xtr, Ytr, Xte);  % predict

figure, hold on
plot(yte_mu,'Color',[.8,0,0],'LineWidth',2);
ysd = 0.5*sqrt(yte_s2);
plot(yte_mu+ysd, 'Color', [.8,0,0]);
plot(yte_mu-ysd, 'Color', [.8,0,0]);
plot(Yte);
legend(sprintf('err: %f\n', mean((Yte-yte_mu).^2)));

x = Xtr;
y = Ytr;
xs = Xte;
nargin = 8;