
build-page:
	git branch -D gh-pages
	git checkout --orphan gh-pages
	cp examples/* .
	mkdir public
	cp -R vendor/* public
	sed -i.bak 's/\.\.\/vendor/public/g' index.html
	sed -i.bak 's/\.\.\///g' index.html
	rm *.bak
	git add . -A
	git commit -m 'Building gh-pages branch'
	git push origin gh-pages --force
	rm -rf public
	git checkout master
