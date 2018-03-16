'use strict';

describe('Dummy test', function(){
  it('should pass without errors', function(done){
    expect(window.r).to.be(undefined);
    expect({ a: 'b' }).to.eql({ a: 'b' });
    expect(5).to.be.a('number');
    expect([]).to.be.an('array');
    expect(window).not.to.be.an(Image);
    done();
  });
});