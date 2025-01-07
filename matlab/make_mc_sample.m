%make_mc_sample: creates a file for use as a user-defined parameter point
%sample for paramotopy.
%
% the various options are set INSIDE the file.
%
% silviana amethyst
% Mathematics
% Colorado State University
% 2011-2013
% silvianaamethyst@gmail.com


function sample = make_mc_sample()

loc = which('make_mc_sample');


num_param = 6;
num_points = 5000;

cube = 0; % make cube, use only first bound interval.
%if cube==0 must have the correct number of entries in bound, namely one
%row per parameter.

%  0.2046    0.7951    0.0071    0.1200    0.6005    0.9237    0.7623    0.0114
bound = [...
	0 0.1;...
	0 0.5;...
	0 1;...
	0.5 1.3;...
	0.3 1.4;...
	0 0.1;...
	];

 
 
if cube==0
	if length(bound(:,1))~=num_param
		display('error: while not using a cube-sample, the number of parameters must\nequal the number of rows in ''bound''');
		display(sprintf('you may edit the config by running:\nopen %s',loc));
		return;
	end
end

%now we sample the box determined by the variable 'bound' 
sample = zeros(num_points,2*num_param);
for ii = 1:num_param
	if cube==1
		sample(:,ii) = (bound(1,2)-bound(1,1)) * rand(num_points,1) + bound(1,1);	
	else
		sample(:,ii) = (bound(ii,2)-bound(ii,1)) * rand(num_points,1) + bound(ii,1);	
	end
end


%write the monte carlo file for paramotopy.

fid = fopen(sprintf('spacesample_%i_%i',num_param,num_points),'w');
fprintf(fid,'%i\n',num_points);
for ii = 1:num_points
	for jj = 1:num_param
		fprintf(fid,'%.15f 0 ',sample(ii,jj));
	end
	fprintf(fid,'\n');
end

fclose(fid);



end
