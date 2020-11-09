#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <time.h>
#include <pwd.h>
#include <grp.h>

using namespace std;

void get_rwx(const struct dirent *item, const struct stat *buf, char *rwx)
{
	switch (item->d_type)
	{
	case DT_DIR:
		rwx[0] = 'd';
		break;
	case DT_SOCK:
		rwx[0] = 's';
		break;
	case DT_FIFO:
		rwx[0] = 'p';
		break;
	case DT_LNK:
		rwx[0] = 'l';
		break;
	case DT_BLK:
		rwx[0] = 'b';
		break;
	case DT_CHR:
		rwx[0] = 'c';
		break;
	default:
		rwx[0] = '-';
		break;
	}

	if (buf->st_mode & S_IRUSR)
		rwx[1] = 'r';
	else
		rwx[1] = '-';

	if (buf->st_mode & S_IWUSR)
		rwx[2] = 'w';
	else
		rwx[2] = '-';

	if (buf->st_mode & S_IXUSR)
		rwx[3] = 'x';
	else
		rwx[3] = '-';

	if (buf->st_mode & S_IRGRP)
		rwx[4] = 'r';
	else
		rwx[4] = '-';

	if (buf->st_mode & S_IWGRP)
		rwx[5] = 'w';
	else
		rwx[5] = '-';

	if (buf->st_mode & S_IXGRP)
		rwx[6] = 'x';
	else
		rwx[6] = '-';

	if (buf->st_mode & S_IROTH)
		rwx[7] = 'r';
	else
		rwx[7] = '-';

	if (buf->st_mode & S_IWOTH)
		rwx[8] = 'w';
	else
		rwx[8] = '-';

	if (buf->st_mode & S_IXOTH)
		rwx[9] = 'x';
	else
		rwx[9] = '-';
		rwx[10] = 0;
}

void printDir(string path, int depth)
{
	DIR *cur = opendir(path.c_str());
	if (cur == NULL)
	{
		cout << "Error opening " << path << endl;
		return;
	}
	chdir(path.c_str());
	dirent *item;
	struct stat info;
	while ((item = readdir(cur)) != NULL)
	{
		if (item->d_name != string(".") && item->d_name != string(".."))
		{
			stat(item->d_name, &info);
			for (int i = 0; i < depth; i++)
				cout << "|————";
			char rwx[11];
			get_rwx(item, &info, rwx);
			struct passwd *pw = getpwuid(info.st_uid);
			struct group *gr = getgrgid(info.st_gid);
			char accessTime[36];
			char modifyTime[36];
			strftime(accessTime, 36, "%Y年%m月%d日 %H:%M:%S", localtime(&info.st_atime));
			strftime(modifyTime, 36, "%Y年%m月%d日 %H:%M:%S", localtime(&info.st_mtime));
			cout << rwx << " ";
			cout << info.st_nlink << " ";
			cout << pw->pw_name << " ";
			cout << gr->gr_name << " ";
			//cout << "inode:" << item->d_ino << " ";
			printf("%8ld ", info.st_size);
			//cout  << info.st_size << " ";
			//cout << "LastAccess:" << accessTime << " ";
			cout << modifyTime << " ";
			cout << item->d_name << endl;
			if (item->d_type == DT_DIR)
			{
				printDir(string(item->d_name), depth + 1);
				chdir("..");
			}
		}
	}
	return;
}

int main(int argc, char **argv)
{
	string path;
	if (argc == 1)
		printDir(".", 0);
	else
		printDir(argv[1], 0);
	//printDir(path.c_str(), 0);
	return 0;
}