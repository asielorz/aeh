namespace aeh::main_loop
{

	template <typename Upstream, typename Update>
	auto LongInteractionLoop<Upstream, Update>::update_impl(LongInteractionUpdateInput loop) -> void
	{
		update_function(loop);
	}

	template <typename Upstream, typename Update>
	auto LongInteractionLoop<Upstream, Update>::render_impl(LongInteractionRenderInput loop) const -> void
	{
		upstream_loop->render(loop, upstream_loop->start_frame());
	}

} // namespace aeh::main_loop
