$(function() {
    // Handler for .ready() called.
    // $('.expand-one').click(function(){
    //     $('.content-one').slideToggle('slow');
    // });

    $(".collapsing-header").click(function () {
        $collapsing_header = $(this);
        $content = $collapsing_header.next();
        $content.slideToggle(500, function () {
            $collapsing_header.text(function () {
                // return $content.is(":visible") ? "Collapse" : "Expand";
            });
        });

    });
});
