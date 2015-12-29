
x = [0:0.1:5; 0:0.1:5]'; 
%x = [0 1 2 3 4 5; 0 1 2 3 4 5]';
rng(10, 'twister');
y = 2*sum(x,2)+randn(size(x, 1), 1);   % generate training data
%meanF = {@meanSum, {@meanLinear, @meanConst}}; % specify mean function
meanF = {@meanLinear};
%cov = {@covMaternard, 5}; 
cov = {@covSEiso};
lik = {@likGauss};  % specify covariance and lik
hyp.cov = [1 1]; %[log(1);log(1.2)]; 
hyp.lik =  1; % log(0.9); 
hyp.mean = [1 1]'; %[3];
par = {meanF,cov,lik,x,y}; mfune = @minimize; % input for GP function

% a) plain marginal likelihood optimisation (maximum likelihood)
inf = {@infExact};                                  % inference method
%hyp_plain = feval(mfun, hyp, @gp, -10, im, par{:}); 

[post nlZ dnlZ] = feval(inf{:}, hyp, meanF, cov, lik, x, y); 
hyp0 = hyp
hyp = minimize(hyp0,'gp', -100, inf, meanF, cov, lik, x, y); % opt hypers


dlmwrite('x.csv', [x y], 'delimiter', ',', 'precision', 9);
csvwrite('aplha.csv', post.alpha);
csvwrite('sW.csv', post.sW);
csvwrite('L.csv', post.L);

f = 'gp';
X = hyp0;
length=-10;
varargin = {inf, meanF, cov, lik, x,y};