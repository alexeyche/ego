
%x = (0:0.1:10)'; 
x = [0 1 2 3 4 5; 0 1 2 3 4 5]';
y = 2*sum(x,2)+randn(size(x, 1), 1);   % generate training data
meanF = {@meanLinear}; % specify mean function
cov = {@covMaternard, 1}; lik = {@likGauss};  % specify covariance and lik
hyp.cov = [1 1 1]; %[log(1);log(1.2)]; 
hyp.lik =  1; % log(0.9); 
hyp.mean = [1 1]'; %[3];
par = {meanF,cov,lik,x,y}; mfun = @minimize; % input for GP function

% a) plain marginal likelihood optimisation (maximum likelihood)
inf = {@infExact};                                  % inference method
%hyp_plain = feval(mfun, hyp, @gp, -10, im, par{:}); 

[post nlZ] = feval(inf{:}, hyp, meanF, cov, lik, x, y); 


display(nlZ);

csvwrite('x.csv', [x y]);
csvwrite('aplha.csv', post.alpha);
csvwrite('sW.csv', post.sW);
csvwrite('L.csv', post.L); 