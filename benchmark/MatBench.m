function n = MatBench;
disp( '===========================')
disp( 'Written by ...')

clear all;
n = input('Enter Matrix size: ');
A = rand(n,n);
B = A*A; % Warmup: load dlls??
totalT = 0.0;

% start tests
tic; B = A*A; t1 = toc;
totalT = totalT + t1;
disp(['Multiply    ', num2str(t1)    ' secs' ])
%
tic; [L, U, P] = lu(A); t2 = toc;
totalT = totalT + t2;
disp(['LU Decomp   '  num2str(t2)    ' secs' ])
tic; B = inv(A); t3 = toc;
totalT = totalT + t3;
disp(['Invert A    '  num2str(t3)    'secs' ])
% 
tic; [U,S,V] = svd(A); t4 = toc; 
totalT = totalT + t4; 
disp(['SDV A '     num2str(t4), 'secs ']) 
% 
tic; [Q,R,P] = qr(A); t5 = toc; 
totalT = totalT + t5; 
disp(['QR Decomp ' num2str(t5)  'secs ']) 
% 
tic; [v,d] = eig(A); t6 = toc; 
totalT = totalT + t6; 
disp(['Eigen A ' num2str(t6) 'secs ']) 
% 
tic; d = det(A); t7 = toc; 
totalT = totalT + t7; 
disp(['DET A ' num2str(t7) 'secs ']) 
% 
tic; r = rank(A); t8 = toc; 
totalT = totalT + t8; 
disp(['Rank A '  num2str(t8) ' secs ']) 
% 
tic; c = cond(A); 
t9 = toc; totalT = totalT + t9; 
disp(['Cond A ' num2str(t9) ' secs ']) 
% 
disp(['Total Time ' num2str(totalT) ' secs']); 
% 


