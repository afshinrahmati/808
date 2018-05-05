$(function() {

	$(".scroll").click(function(e) {
		e.preventDefault();
		$('html, body').animate({
			scrollTop: ($($(this).attr('href')).offset().top)
		}, 700);
	});	
	
	$("ul.navigat li").click(function(e) {
		e.preventDefault();
		$('html, body').animate({
			scrollTop: ($('#'+ $(this).attr('to-id')).offset().top)
		}, 1000);
	});
   
    AOS.init({
      offset: 200,
      duration: 600,
      easing: 'ease-in-sine',
      delay: 100,
    });

    
    $('.vip').click(function(){
        $('.vip-div').toggleClass('open').slideToggle()
    })
    $('.point').click(function(){
        $('.point-div').toggleClass('open').slideToggle()
    })

});
