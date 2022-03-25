#ifndef		CGIPROCESS_HPP
# define	CGIPROCESS_HPP

# include	<cstdlib>
# include	<cstring>
# include	<string>
# include	<unistd.h>
# include	<signal.h>
# include	<map>
# include	"HTTPData.hpp"
# include	"ConfigBlocks.hpp"
# include	"FileController.hpp"

extern NginxConfig::GlobalConfig
	_config;

class
	CGIProcess
{
	private:
		char**
			envp;
		char *
			argv[4];
		int
			inputPair[2],
			outputPair[2];
		int
			env_size;
		std::string
			only_root,
			only_file;
		pid_t
			_pid;

	public:
		CGIProcess(
				HTTPData* data);

		~CGIProcess(
				void);


		// execve 1번째 파라미터
		char*
			getFilePath(
				void);
		// execve 2번째 파라미터
		char** 
			getArgv(
				void);
		// execve 3번째 파라미터, setEnvp의 후속으로
		char**
			generateEnvp(
				std::map<std::string, std::string>	env);

		void
			setCGIArgv(
				HTTPData* data);

		void
			setEnvp(
				HTTPData* data);

		//파이프는 1로 들어가서 0으로 나온다
		/*
		 * 				파이프A[1] -----------------> 파이프A[0]
		 *		메인												자식
		 *				파이프B[0] <----------------- 파이프B[1]
		 * */
		int&
			getInputPair(
				void);

		int&
			getOutputPair(
				void);

		//얜 타겟 없어도 되는건가
		//그건 POST메소드에서 타겟이 있다면 해당 타겟을fd화 시켜서 인자로 넣으면 됨
		//킹치만 그건 없었으니...
		void
			run(
				void);
};

#endif
