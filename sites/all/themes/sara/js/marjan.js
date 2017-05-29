var $ = jQuery;
$(document).ready(function () {
    
    /*erasing pane-uc-order when its empty*/
    if (!$('.page-user .pane-uc-orders-user .view-uc-orders-user div').length) {
        $('.pane-uc-orders-user').remove();
    }
    
    /*adjust position of bookmark icon and admin-tabs in node-page*/
    $('.page-node:not(.page-gallery):not(.page-shop):not(.page-pedia) div.tabs').css({'top':$('#page-title').height()});
    
    /*shop*/
    $('.node.node-product.view-mode-full .group-left > .display-price,.node.node-product-kit.view-mode-full .group-left > .display-price').prepend('<span>قیمت نهایی:</span>');
    
    /*ancher of pedia node*/
    if($('.page-node.page-pedia .field-type-node-reference').length){
        $('.page-node.page-pedia .view-mode-full .group-anchor > div').prepend('<div class="divanchors"></div>');
        $('.page-node.page-pedia .flag-outer-bookmarks').css({'top': 91});
        $('.page-node.page-pedia .field-type-node-reference').each(function(){
            var anchclass = $(this).attr('class').split(" ")[1].substr(17);
            var spananch = $(this).find('.field-label').text().slice(0,-2);
            $('.divanchors').append('<a class="'+ anchclass +'" href="#'+anchclass+'"><span>'+spananch+'</span></a>');
            $(this).attr('id', anchclass);
        });
    }
    $('.divanchors a').click(function(e) {
        e.preventDefault();
        $('html, body').animate({
            scrollTop: $($(this).attr('href')).offset().top - 50
        }, 700); 
    });
    
    /*closing landing-job submittion*/
    $('.page-node-14314 #webform-client-form-14314 .form-actions,.page-node-14314 #webform-client-form-14314  .captcha').remove();
    
    
    
});






