
seed = 5; randn('seed',seed), rand('seed',seed)

ntr = 10; nte = 1e2;                        % number of training and test points
xtr = 10*sort(rand(ntr,1));                                     % sample dataset
f = @(x) sin(x)+sqrt(x);                            % "true" underlying function
sn = 0.2;
ytr = f(xtr) + randn(ntr,1)*sn;                             % add Gaussian noise
i = randperm(ntr); nout = 3;                                      % add outliers
ytr(i(1:nout)) = 5;
% xte = [0 1.0 5.0 7.0 10]';
xte = linspace(0, 10, nte)';                    % support, we test our function on

mean = {@meanSum, {@meanLinear, @meanConst}}; % specify mean function
%meanF = {@meanLinear};
%cov = {@covMaternard, 5}; 
cov = {@covGaborard};
%cov = {@covMaterniso, 5};
%cov = {@covMaternard, 1};
%cov = {@covSEiso};
%cov = {@covSEard};

%lik = {@likGauss}; hyp.lik =  1;
%lik = {@likT}; hyp.lik = [1 1];
lik = {@likGamma, 'exp'}; ytr = abs(ytr); hyp.lik = [1.0 ];

hyp.cov = [1 1]; %[log(1);log(1.2)]; 
hyp.mean = [1 1]; %[3];

%inf = {@infExact};                                  % inference method
inf = {@infLaplace};

%[lp ymu ys2] = feval(lik{:}, 

[post nlZ dnlZ] = feval(inf{:}, hyp, mean, cov, lik, xtr, ytr);

hyp = minimize(hyp,'gp', -100, inf, mean, cov, lik, xtr, ytr); % opt hypers

[yte_mu, yte_s2] = gp(hyp, inf, mean, cov, lik, xtr, ytr, xte);  % predict

%cputime-t
dlmwrite('x.csv', [xtr ytr], 'delimiter', ',', 'precision', 9);
dlmwrite('x_test.csv', [xte], 'delimiter', ',', 'precision', 9);

figure, hold on
plot(xte,yte_mu,'Color',[.8,0,0],'LineWidth',2)
ysd = 0.5*sqrt(yte_s2);
fill([xte;flipud(xte)],[yte_mu+ysd;flipud(yte_mu-ysd)], [.8, 0, 0],'EdgeColor', [.8, 0, 0],'FaceAlpha',0.1,'EdgeAlpha',0.3);
plot(xtr,ytr,'k+'), plot(xtr,ytr,'ko');

x = xtr;
y = ytr;
xs = xte;
nargin = 8;

