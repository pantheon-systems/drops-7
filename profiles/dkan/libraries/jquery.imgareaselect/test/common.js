function dim(element, shift) {
    if (!shift) shift = 0;
    
    return {
        x1: Math.round($(element).offset().left + shift),
        y1: Math.round($(element).offset().top + shift),
        x2: Math.round($(element).offset().left + $(element).width() + shift),
        y2: Math.round($(element).offset().top + $(element).height() + shift)
    };
}

function testCleanup() {
    $('#t').empty();
}
