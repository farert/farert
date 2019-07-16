

　すべてWindows上にPython3をインストールして実行していた。
　実行するには、このディレクトリで実行しても良いものもあるが、
　多くは、一つ上のdbディレクトリで実行する。
　　例；
     $ python scripts/comapnyfare.py 盛岡 好摩

  参照データベースは、環境変数 farertDB にdbパスを設定すること
  WindowsでなくMAC-OSなどのUnix環境でも動作できるようにした(2019.Mar)

  $ export farertDB=../jrdb2019.db

以下のエラーはpython2系では起きるので、python3でおこなうこと
　sqlite3.ProgrammingError: You must not use 8-bit bytestrings unless you use a text_factory that can interpret 8-bit bytestrings (like text_factory = str). It is highly recommended that you instead just switch your application to Unicode strings.

"TypeError: 'str' object is not callable"
以上のエラーは、python2, python3でも出るが、原因は、以下。
con.text_factory = '日本語'   # MAX-OSでUTF-８日本語を使用する場合必要
が、含まれている場合
python2系では必要だがPython3系では日本語文字列返すクエリで必要になる。
