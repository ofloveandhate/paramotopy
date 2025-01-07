% gather_data_paramotopy(filename)
%
%captures generated data from a paramatopy run set up via the
%maketestpolysaurus function.
%
% dependencies: parse_input
%               getlinecount
%               is_last_line_empty

%silviana amethyst
%colorado state university
%mathematics
%silvianaamethyst@gmail.com



function gather_data_paramotopy(filename)



if nargin==0
    filename = input('filename of paramotopy input file\n','s');
end

if isempty(dir(filename))
	display('no file of that name in this directory');
	return
end


[info] = parse_input(filename);

run = get_run(filename);



[folders, numfolders] = getfolders(filename,run);

display(sprintf('gathering data starting from %s',folders{1}));
fid = fopen(sprintf('bfiles_%s/run%i/step1/nonsingular_solutions',filename,run),'r');
generic_num_solns = fscanf(fid,'%i',[1,1]);
fclose(fid);



dataname = detectdatasets(filename,run);

tic;

if ~info.userdefined

	sizes = info.paramvalues(:,5)';
	nsolns = zeros([ sizes 1]); %used inside eval calls



	canonicallinenumber = zeros(1);
	canonicallocation = zeros(1,2*info.numparam);
	base_soln = zeros(info.numvar,2); %the prototypical solution for this system
	canonicalsoln = repmat(struct('indsoln',base_soln),[generic_num_solns,1]);
	solutions = repmat(struct('clusterofsoln',canonicalsoln),[sizes 1]);
	locations = repmat(struct('location',canonicallocation,'line_number_mc',canonicallinenumber),[sizes 1]);

else
	

	sizes = getlinecount(info.userparameterfile)-1; % subtract 1 for the header line

	nsolns = zeros([ sizes 1]); %used inside eval calls
	
	canonicallinenumber = zeros(1);
	canonicallocation = zeros(1,2*info.numparam);
	base_soln = zeros(info.numvar,2); %the prototypical solution for this system
	canonicalsoln = repmat(struct('indsoln',base_soln),[generic_num_solns,1]);
	solutions = repmat(struct('clusterofsoln',canonicalsoln),[sizes 1]);
	locations = repmat(struct('location',canonicallocation,'line_number_mc',canonicallinenumber),[sizes 1]);
	
	info.mc_lines = sizes;

	
end




indices = '';
for ii = 1:info.numparam
	indices = sprintf('%sI%i,',indices,ii);
end
indices = indices(1:end-1);%nip off the last comma


tmp = base_soln;
for ii = 1:numfolders
    dirlist = dir([folders{ii} '/' dataname '*']);
    display(folders{ii})
    for jj = 1:length(dirlist)

	linecount = getlinecount([folders{ii} '/' dirlist(jj).name]);


        display(dirlist(jj).name)
        fid = fopen([folders{ii} '/' dirlist(jj).name]);
        params = fgetl(fid);

        current_line_number = 2;

		while (current_line_number<linecount && linecount>2)

            line_number_mc = fscanf(fid,'%i',[1,1]);
			if (mod(line_number_mc+1,1000)==0)
				display(sprintf('solution %i\n',line_number_mc+1));
			end
			location = fscanf(fid,'%f',[1,2*info.numparam]);
			fgetl(fid); % burn a line, since there is a stupid dangling space after the parameter values

			if strcmp(dataname,'failed_paths')
				next_line = fgetl(fid);
				if isempty(next_line)
					numrealsolns = 0;
				else
					numrealsolns = sscanf(next_line,'%i',[1,1]);
				end
			else
				% there must be a number next, telling how many solutions there are at this parameter point
				numrealsolns = fscanf(fid,'%i',[1,1]);
			end
            


			
            eval( ['[' indices '] = ind2sub(sizes,line_number_mc+1);']);
            eval( ['nsolns(' indices ') = numrealsolns;	']);
			eval(['locations(' indices ').line_number_mc = line_number_mc;']);
            eval(['locations(' indices ').location = location;']);

            
            current_line_number = current_line_number+4;
            for kk = 1:numrealsolns
				for mm = 1:info.numvar
                    tmp(mm,:) = fscanf(fid,'%f %f\n',[1,2]);
                    
				end

				eval( ['solutions(' indices ').clusterofsoln(kk).indsoln = tmp;']);
            end
            current_line_number = current_line_number+numrealsolns*(info.numvar+1);
		end
		
		
		
        fclose(fid);
    end
	
end

toc
display('done gathering data.  writing to disk');

% a = whos('solutions');
% if a.bytes>104857600
% 	for ii = 0:99
% 		ii_local = ii*10+1;
% 		smaller_solns = solutions(ii_local:ii_local+9,:);
% 		save([dataname 'smaller' num2str(ii) '.mat'],'ii','ii_local','nsolns','smaller_solns','info','locations','generic_num_solns','-v7.3');
% 	end
% else

	save([dataname 'datamatlab.mat'],'nsolns','solutions','info','locations','generic_num_solns','-v6');
%end


save([dataname '_nsolns.mat'],'nsolns','info','locations','generic_num_solns','-v6')

%switch info.userdefined
%	case {0}
%		[data,orders] = assemble_data(solutions,info,generic_num_solns,nsolns);
%	case {1}
%		[data,orders] = assemble_data_userdef(solutions,info,generic_num_solns,nsolns);
%end
%save(['assembleddata.mat'],'data','orders','info','-v7.3');
end


%%%%%%%%%%%%%%%%%%%%%%%
%%%
%%%   subfunctions
%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%

function dataname = detectdatasets(filename,run)

if ~isempty(dir(sprintf('bfiles_%s/run%i/gathered_data/finalized',filename,run)))
	
	datadir = sprintf('bfiles_%s/run%i/gathered_data/finalized',filename,run);
else
	display('no gathered data detected.  trying to get at the raw data\n');
	datadir = sprintf('bfiles_%s/run%i/step2/DataCollected/c1',filename,run);
end
possiblefiles = dir([datadir '/*0']);  %lists all of the first data files.

display('The following saved data types have been detected');
for ii = 1:length(possiblefiles)
	
	display(sprintf('%i: %s',ii,possiblefiles(ii).name(1:end-1)));
end

choice = input('which data file type?\n');
dataname = possiblefiles(choice).name(1:end-1);

end



function [folders, numfolders] = getfolders(filename,run)

if ~isempty(dir(sprintf('bfiles_%s/run%i/gathered_data/finalized',filename,run)))
	
	folders{1} = sprintf('bfiles_%s/run%i/gathered_data/finalized',filename,run);
	numfolders = 1;
else
	fid = fopen(sprintf('bfiles_%s/run%i/folders',filename,run));
	tmp = fscanf(fid,'%s\n',[1 1]);
	folders{1} = tmp;
	numfolders = 1;
	while ~strcmp(tmp,'')

		tmp = fscanf(fid,'%s\n',[1 1]);
		if ~strcmp(tmp,'')
			numfolders = numfolders+1;
			folders{numfolders} = tmp;
		end
	end
	fclose(fid)
end



end

function [run] = get_run(filename)

runs = dir(sprintf('bfiles_%s/run*',filename));
counter = 0;
for ii = 1:length(runs)
	if ~isempty(dir(sprintf('bfiles_%s/%s/',filename,runs(ii).name)))
		counter = counter+1;
		completed_runs{counter} = runs(ii).name;
	end
end

if length(completed_runs)>1

	display('found these runs:\n');
	for ii = 1:counter
		display(sprintf('%i: %s',ii,completed_runs{ii}));
	end

	run_choice = input('choose\n: ');

	run_choice = completed_runs{run_choice};
else
	run_choice = completed_runs{1};
end
run = str2num(run_choice(4:end));



end
