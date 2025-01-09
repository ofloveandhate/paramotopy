%intended as a generic method to plot data from a paramatopy run.  probably
%not as specific as needed for specialized-application plots.

%first, you must run gather_data_paramotopy('filename')
%to make the .mat structures in the directory.

%silviana amethyst
%Mathematics
%Colorado State University
%2011-2013
%silvianaamethyst@gmail.com

%this code is a continual work in progress.  if you find errors problems or
%room for improvement, please let me know.

function [] = plot_paramotopydata()
global fontsize window format format_flag info dataname movieformat movie_window


fontsize = 20;
window = [40 40 800 600];
movie_window = [20 20 480 420];

format = 'eps';
format_flag = 'epsc2';

movieformat = 'stills';   %options are 'avi', and 'stills', where stills writes jpgs to a folder for a different program to compile to a movie.
reset(gcf);

%gets data from gather_data_paramatopy:  example of stuff
%

%   generic_num_solns        1x1                  8  double              
%   info                     1x1               1802  struct 
	%          numeqn: 3
	%     numvargroup: 1
	%        numparam: 1
	%        numconst: 0
	%             eqn: {'a-x^2-y^2-z^2'  'x+y-z^2'  'y-z'}
	%            vars: {'x'  'y'  'z'}
	%     userdefined: 0
	%      paramnames: {'a'}
	%     paramvalues: [0 0 1 0 400]
	%          numvar: 3
%   locations              400x1              57728  struct 
	% 400x1 struct array with fields:
	%     location
	%     line_number_mc
%   nsolns                 400x1               3200  double              
%   solutions              400x1             222464  struct 
	% 400x1 struct array with fields:
	% 	clusterofsoln
	
dataname = detectdatasets();

if isempty(dataname)
	display('found no data');
	return;
end

posreal = input('work with only positive?\n 1 yes, 0 no\n: ');

if posreal==1
	assembledname = sprintf('posrealassembleddata_%s.mat',dataname);
else
	assembledname = sprintf('assembleddata_%s.mat',dataname);
end

if isempty(dir(assembledname))

	display('assembling data for plotting');


	info = [];
	load(dataname);

	whos


	if posreal==1
		switch info.userdefined
		case {0}
			[data,orders] = assemble_data_real(solutions,info,generic_num_solns,nsolns);
		case {1}
			[data,orders] = assemble_data_userdef_real(solutions,info,generic_num_solns,nsolns);
	end
	else
		switch info.userdefined
			case {0}
				[data,orders] = assemble_data(solutions,info,generic_num_solns,nsolns);
			case {1}
				[data,orders] = assemble_data_userdef(solutions,info,generic_num_solns,nsolns);
		end
	end
	save(assembledname,'data','orders','info','-v6');
%what is orders?  it has the order the solve was performed in... the order
%in the mc file generated.
else
	load(assembledname);

	if posreal==1
		load(dataname,'locations');
		load('posrealnsolns.mat','nsolns');
		
		
	else

		load(dataname,'nsolns','locations');
	end
end


plotmode = get_mode_from_user(info);



switch(plotmode)
	case{'solutions'}
		
		colormap jet
		switch(info.numvar)
			case{1}
				plot_1solns(data,orders)
			case{2}
				plot_2solns(data,orders)
			case{3}
				
				plot_3solns(data,orders)
			otherwise 

				nplot = getuserindicestoplot_vars(info);
				plot_higher_solns(data,orders,nplot);

		end

	case{'num_solns'}
		plot_nsolns(info,nsolns,locations)
	otherwise
		
end%re: switch(plotmode)


render_into_file(plotmode,dataname)


end%re:main function plot_paramotopydata



%%%%%%%%%%%%%
%%%
%%%  subfunctions
%%%
%%%%%%%%%%%%%


function plot_nsolns(info,nsolns,locations)
global fontsize dataname movieformat format format_flag movie_window

maxiii = max_recursive(nsolns);

if maxiii~=0
	nsolns = nsolns / maxiii;
end

switch info.userdefined
	case {0}

		sizes = info.paramvalues(:,5)';
		nonsingleton = sizes>1;
		num_nonsingleton = sum(nonsingleton);
		possibilities = 1:info.numparam;
		possibilities = possibilities(nonsingleton);
		switch num_nonsingleton
			case{1}
				%this case untested so far
				index = possibilities(1);
				bar(linspace(info.paramvalues(index,1),info.paramvalues(index,3),info.paramvalues(index,5) ),squeeze(nsolns));
			case{2}
				%this case moderately untested so far



				index1 = possibilities(1);
				index2 = possibilities(2);


				linspace1 = linspace(info.paramvalues(index1,1),info.paramvalues(index1,3),info.paramvalues(index1,5) );
				linspace2 = linspace(info.paramvalues(index2,1),info.paramvalues(index2,3),info.paramvalues(index2,5) );

				[X,Y] = meshgrid(linspace1,linspace2);

				pcolor(X,Y,squeeze(nsolns)');
				make_nsolns_colorbar(maxiii,fontsize);
				xlabel(info.paramnames{index1});
				ylabel(info.paramnames{index2});
			otherwise

				%have enough to make a movie!!!
				index = find(nonsingleton==1,num_nonsingleton,'first');


				display('these parameters had nonsingleton dimensions in your mesh:\n')

				for ii = 1:num_nonsingleton
					display(sprintf('%s: %i mesh points',info.paramnames{index(ii)},info.paramvalues(index(ii),5)));
				end
				
				movie_bool = input('\nmake a movie?  if not movie, this will make a still plot.  1 yes, 0 no.\n:');
				
				
				
				if movie_bool == 1
					display('choose parameter to be on X-axis:')
					for ii = 1:num_nonsingleton
						display(sprintf('%s: %i mesh points, %i',info.paramnames{index(ii)},info.paramvalues(index(ii),5),ii));
					end
					x_choice = input(':');
					x_param = index(x_choice);



					avail = [index(1:x_choice-1) index(x_choice+1:end)];
					display('choose parameter to be on Y-axis:')
					for ii = 1:num_nonsingleton-1
						display(sprintf('%s: %i mesh points, %i',info.paramnames{avail(ii)},info.paramvalues(avail(ii),5),ii));
					end
					y_choice = input(':');
					y_param = avail(y_choice);

					if num_nonsingleton>3
						display('choose time parameter for movie');
						avail = [index(1:y_choice-1) index(y_choice+1:end)];
						for ii = 1:num_nonsingleton-2
							display(sprintf('%s: %i mesh points, %i',info.paramnames{avail(ii)},info.paramvalues(avail(ii),5),ii));
						end
						time_choice = input(':');
						time_param = avail(time_choice);

						%this case untested
					else
						time_param = avail(mod(y_choice,2)+1);
					end
					display(sprintf('x-axis: %s\ny-axis: %s\ntime: %s',info.paramnames{x_param},info.paramnames{y_param},info.paramnames{time_param}));
	
					
					linspace_x = linspace(info.paramvalues(x_param,1),info.paramvalues(x_param,3),info.paramvalues(x_param,5) );
					linspace_y = linspace(info.paramvalues(y_param,1),info.paramvalues(y_param,3),info.paramvalues(y_param,5) );
	
					linspace_time =  linspace(info.paramvalues(time_param,1),info.paramvalues(time_param,3),info.paramvalues(time_param,5) );
					[X,Y] = meshgrid(linspace_x,linspace_y); %X,Y used in an eval call.
					h = [];
					set(gcf,'Position',movie_window);


					switch movieformat
						case {'avi'}
							aviobj = avifile(sprintf('nsolns_%s.avi',dataname),'compression','None','fps',10);

						case {'stills'}
							folder = 'nsolnsmoviefiles';
							folder = increment_name(folder);
							mkdir(folder);
						otherwise 
							display('bad movie format up top');
							return;
					end
					
					run_me = 'h = pcolor(X'',Y'',squeeze(nsolns(';
					for ii = 1: info.numparam
						if ii == time_param
							printchar = 'ii';
						else
							if sizes(ii)>1
								if or(x_param==ii, y_param==ii)
									printchar =  ':';
								else
									printchar = '1';
								end
							else
								printchar = ':';
							end
						end
						if ii ~= info.numparam
							printchar = sprintf('%s,',printchar);
						end
						run_me = sprintf('%s%s',run_me,printchar);%[run_me printchar];

					end
					if x_param > y_param  %transpose to get correct dimensions
						run_me = [run_me ''''];
					end
					run_me = [run_me ')));']; %close the parenthesees

					for ii = 1:info.paramvalues(time_param,5)
							fig = figure(gcf);
							

							eval(run_me);%essentially runs h = pcolor(X',Y',data);
							make_nsolns_colorbar(maxiii,fontsize);
							xlabel(info.paramnames{x_param})
							ylabel(info.paramnames{y_param})
							
							if maxiii~=0
								set(h,'EdgeColor','None');
							end
							
							title(sprintf('%s = %1.3f',info.paramnames{time_param},linspace_time(ii)));
							drawnow
							
							switch movieformat
								case {'avi'}
								
									F = getframe(fig);
									aviobj = addframe(aviobj,F);
								case {'stills'}
									renderedpicname = sprintf('%s/frame%i.%s',folder,ii,'jpg');
									print(gcf,renderedpicname,sprintf('-d%s','jpeg'),'-r150')
							end
					end

					if strcmp(movieformat,'avi')
						aviobj = close(aviobj);  % you must assign aviobj as both input and output...
					end
					
				

				else%re: if moviebool
					
					if info.numparam==3

						locations = reshape(locations,[],1);
						locations_to_plot = zeros(length(locations),3);
						for ii=1:length(locations)
							locations_to_plot(ii,:) = locations(ii).location([1 3 5]);
						end

						h = scatter3(locations_to_plot(:,1),locations_to_plot(:,2),locations_to_plot(:,3),20,reshape(nsolns,[],1) );

						xlabel(info.paramnames{1},'FontSize',fontsize);
						ylabel(info.paramnames{2},'FontSize',fontsize);
						zlabel(info.paramnames{3},'FontSize',fontsize);
					else
						
						[nplot] = getuserindicestoplot_param(info);

						locations = reshape(locations,[],1);
						locations_to_plot = zeros(length(locations),nplot.userparams);
						for ii=1:length(locations)
							locations_to_plot(ii,:) = locations(ii).location(2*nplot.indices-1);
						end
						
						switch(nplot.userparams)
							case 2
								h = scatter(locations_to_plot(:,1),locations_to_plot(:,2),20,reshape(nsolns,[],1) );
							case 3
								h = scatter3(locations_to_plot(:,1),locations_to_plot(:,2),locations_to_plot(:,3),20,reshape(nsolns,[],1) );
							otherwise
								display('sorry, not equipped to plot that number of parameters yet.');
								return;
						end
					end
				end%re: if moviebool

		end

	case {1} % user-defined parameter sample

		switch info.numparam


			case {1}
				locations_to_plot = zeros(info.mc_lines,1);
				for ii=1:info.mc_lines
					locations_to_plot(locations(ii).line_number_mc+1) = locations(ii).location(1);
				end
				display('this case around line 366 undeveloped. \n');
				return;
				%undeveloped to here in this case
			case {2}
				locations_to_plot = zeros(info.mc_lines,2);
				for ii=1:info.mc_lines
					locations_to_plot(locations(ii).line_number_mc+1,:) = locations(ii).location([1 3]);
				end
				h = scatter(locations_to_plot(:,1),locations_to_plot(:,2),20,nsolns);

				xlabel(info.paramnames{1},'FontSize',fontsize);
				ylabel(info.paramnames{2},'FontSize',fontsize);
			case {3}

				locations_to_plot = zeros(info.mc_lines,3);
				for ii=1:info.mc_lines
					locations_to_plot(locations(ii).line_number_mc+1,:) = locations(ii).location([1 3 5]);
				end

				h = scatter3(locations_to_plot(:,1),locations_to_plot(:,2),locations_to_plot(:,3),20,nsolns);

				xlabel(info.paramnames{1},'FontSize',fontsize);
				ylabel(info.paramnames{2},'FontSize',fontsize);
				zlabel(info.paramnames{3},'FontSize',fontsize);
			otherwise  %more than 3 variables
				[nplot] = getuserindicestoplot_param(info);

				

				
				locations_to_plot = zeros(info.mc_lines,nplot.userparams);
				for ii=1:info.mc_lines
% 					locations_to_plot(locations(ii,:).line_number_mc+1,:)
% 					locations(ii).location(nplot.indices*2-1)
					locations_to_plot(locations(ii,:).line_number_mc+1,:) = locations(ii).location(nplot.indices*2-1);
				end

				
				skip = 1;
				if info.mc_lines>10000
					display(sprintf('the sample to plot has %i points.\n',info.mc_lines));
					skip = input('by what integer factor would you like to reduce the size of the sample?\n1 is no reduction (the full data set)\n: ');
					
				end
				
				locations_to_plot = locations_to_plot(1:skip:end,:);
				nsolns = nsolns(1:skip:end);
				
				
				switch nplot.userparams


					case {1}
						%this is untested
						bar(locations_to_plot,nsolns);
						xlabel(info.paramnames{nplot.indices(1)},'FontSize',fontsize);

					case {2}


						h = scatter(locations_to_plot(:,1),locations_to_plot(:,2),20,nsolns);

						xlabel(info.paramnames{nplot.indices(1)},'FontSize',fontsize);
						ylabel(info.paramnames{nplot.indices(2)},'FontSize',fontsize);
					case {3}



						h = scatter3(locations_to_plot(:,1),locations_to_plot(:,2),locations_to_plot(:,3),20,nsolns);

						xlabel(info.paramnames{nplot.indices(1)},'FontSize',fontsize);
						ylabel(info.paramnames{nplot.indices(2)},'FontSize',fontsize);
						zlabel(info.paramnames{nplot.indices(3)},'FontSize',fontsize);
					otherwise  %more than 3 variables
				end

		end




	otherwise


end  %re: switch userdefined

make_nsolns_colorbar(maxiii,fontsize);


figure(gcf);
end%re: function plot_nsolns()





function render_into_file(plotmode,dataname)
global fontsize window format format_flag 


fig1 = gcf;

set(fig1,'Position',window,'PaperPositionMode','auto');
set(gca,'FontSize',fontsize-2);

basename = sprintf('%s_%s',plotmode,dataname(1:end-4));
keepgoing = 1;
while (keepgoing==1)
	currname = increment_name(basename);
	nameforfile = sprintf('%s.%s',currname,format);
	print(fig1,nameforfile,sprintf('-d%s',format_flag));
	keepgoing = input('save another view of this plot? \n if so, set the window, and press 1 then enter.');
end
end

%get whether to plot the number of solutions, or display the real solutions
%themselves.  can only plot nsolns for a mesh-style set of parameter values

function plotmode = get_mode_from_user(info)


	plotmode = -1;
	opts = [1 2];
	while ( isempty(find(plotmode==opts, 1)) )
		plotmode = input('choose:\n1) plot number of solutons on a grid\n2) plot solutions themselves\n:');
	end

	switch plotmode
		case{1}
			plotmode = 'num_solns';
		case{2}
			plotmode = 'solutions';
		otherwise
	end



end




%%%
%%%    data manipulation whatnot.
%%%










function dataname = detectdatasets(style)


if nargin == 0
	style = 'datamatlab.mat';
end


possiblefiles = dir(['*' style]);  %lists all gathered data files.

if length(possiblefiles)>1

	display('The following gathered data types have been detected');
	for ii = 1:length(possiblefiles)

		display(sprintf('%i: %s',ii,possiblefiles(ii).name(1:end-14)));
	end

	choice = input('which data file type?\n');

elseif length(possiblefiles)==1
	choice = 1;
else
	choice = 0;
	
end

if choice ~=0
	dataname = possiblefiles(choice).name;
else
	dataname = [];
end

end








%%%
%%%               plotting functions
%%%


function plot_1solns(data,orders, nplot)
arguments
    data
    orders
    nplot = struct('uservars',1,'indices',1:1)
end

scatter(1,ones(length(data(:,nplot.indices(1)))),data(:,nplot.indices(1)));


end



function plot_2solns(data,orders,nplot)
arguments
    data
    orders
    nplot = struct('uservars',2,'indices',1:2)
end


scatter(data(:,nplot.indices(1)),data(:,nplot.indices(2)),5,orders);

end



function plot_3solns(data,orders,nplot)
arguments
    data
    orders
    nplot = struct('uservars',3,'indices',1:3)
end


scatter3(data(:,nplot.indices(1)),data(:,nplot.indices(2)),data(:,nplot.indices(3)),20,orders);

%want some clever coloring scheme...
end

function plot_4solns(data,orders,nplot)
arguments
    data
    orders
    nplot = struct('uservars',4,'indices',1:4)
end


	scatter3(data(:,nplot.indices(1)),data(:,nplot.indices(2)),data(:,nplot.indices(3)),20,data(:,nplot.indices(4)));
end


function plot_6solns(data,orders,nplot)
arguments
    data
    orders
    nplot = struct('uservars',6,'indices',1:6)
end



	data(:,4) = (data(:,4) - min(data(:,4))) / (max(data(:,4))- min(data(:,4)));
	data(:,5) = (data(:,5) - min(data(:,5))) / (max(data(:,5))- min(data(:,5)));
	data(:,6) = (data(:,6) - min(data(:,6))) / (max(data(:,6))- min(data(:,6)));
	data(:,4:6) = real(data(:,4:6));
	scatter3(data(:,1),data(:,2),data(:,3),20,[data(:,4) data(:,5) data(:,6) ] );
end

function plot_higher_solns(data,orders,nplot)
global info fontsize

switch nplot.uservars
	case{1}
		plot_1solns(data,orders,nplot)
		xlabel(info.vars{nplot.indices(1)},'FontSize',fontsize)

	case{2}
		plot_2solns(data,orders,nplot)
		xlabel(info.vars{nplot.indices(1)},'FontSize',fontsize)
		ylabel(info.vars{nplot.indices(2)},'FontSize',fontsize)

	case{3}
		plot_3solns(data,orders,nplot)
		xlabel(info.vars{nplot.indices(1)},'FontSize',fontsize)
		ylabel(info.vars{nplot.indices(2)},'FontSize',fontsize)
		zlabel(info.vars{nplot.indices(3)},'FontSize',fontsize)

		cbar = colorbar;
		set(get(cbar,'ylabel'),'String', 'order of solve','FontSize',fontsize-2);

	case{4}
		plot_4solns(data,orders,nplot)
		xlabel(info.vars{nplot.indices(1)},'FontSize',fontsize)
		ylabel(info.vars{nplot.indices(2)},'FontSize',fontsize)
		zlabel(info.vars{nplot.indices(3)},'FontSize',fontsize)

		cbar = colorbar;
		set(get(cbar,'ylabel'),'String', info.vars{nplot.indices(4)},'FontSize',fontsize-2);

	case{6}
		plot_6solns(data,orders,nplot)
		xlabel(info.vars{nplot.indices(1)},'FontSize',fontsize)
		ylabel(info.vars{nplot.indices(2)},'FontSize',fontsize)
		zlabel(info.vars{nplot.indices(3)},'FontSize',fontsize)


	otherwise 
		display('too many');

		%want some clever coloring scheme...
end %re: switch
figure(gcf)  %bring it to the front 
end




